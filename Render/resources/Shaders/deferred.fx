#include "Helpers.fx"
texture2D colorBuffer;
texture2D lightBuffer;
texture2D screenBuffer_0 < int width = -1; int height = -1; >;
texture2D screenBuffer_1 < int width = 512; int height = 512; >;
texture2D screenBuffer_2 < int width = 512; int height = 512; >;
texture2D screenBuffer_3 < int width = 512; int height = 512; >;
texture2D screenBuffer_4 < int width = -1; int height = -1; >;
texture2D screenBuffer_5 < int width = 256; int height = 256; >;
texture2D screenBuffer_6 < int width = 256; int height = 256; >;
texture2D screenBuffer_7 < int width = 256; int height = 256; >;
texture2D screenBuffer_8 < int width = -1; int height = -1; >;
texture2D screenBuffer_9 < int width = 256; int height = 256; >;
texture2D screenBuffer_10 < int width = 128; int height = 128; >;
texture2D screenBuffer_11 < int width = 1; int height = 1; >;
texture2D screenBuffer_12 < int width = -1; int height = -1; >;
texture2D normalSpecBuffer;
texture2D shadowDepthBuffer;
texture2D noise;
texture2D test;
texture2D test2;
texture2D test3;
texture2D test4;
textureCUBE cubemap;
float3 lightDirection;
float4 sLP;
float4	ScreenSize;
float4x4 gmWorldViewProj;
float4x4 gmViewProj;
float4x4 gmProj;
float4x4 gmInvProj;
float4x4 gmViewInv;
float3x3 gmView;
float4x4 gmViewProjInv;
float4x4 gmWorld;
static const int CascadeCount = 4;
float4x4 gmLightViewProj[CascadeCount];
float4 SunColor;
float4 gvAmbientColor;
float4 gvAmbientColor2;
float PointLightRange;
float ScreenSizeX;
float ScreenSizeY;
float3 PointLightColor;
float3 ShadowParams; // Shadow Size, 1/Shadow Size, Shadow Bias
int PostProcessCount = 0;
sampler2D colorGBUFF = sampler_state
{
   Texture = <colorBuffer>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Wrap;
   AddressV = Wrap;
};
sampler2D lightBUFF = sampler_state
{
   Texture = <lightBuffer>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Wrap;
   AddressV = Wrap;
};
sampler2D nsGBUFF = sampler_state
{
   Texture = <normalSpecBuffer>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D shadGBUFF = sampler_state
{
   Texture = <shadowDepthBuffer>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D noiseSampler = sampler_state
{
   Texture = <noise>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Wrap;
   AddressV = Wrap;
};
sampler2D testSampler = sampler_state
{
	Texture = <test>;
	MinFilter = POINT;  
	MagFilter = POINT;
	MipFilter = None;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = 0xFFFFFF;
};
sampler2D test2Sampler = sampler_state
{
	Texture = <test2>;
	MinFilter = LINEAR;  
	MagFilter = LINEAR;
	MipFilter = None;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = 0xFFFFFF;
};
sampler2D test3Sampler = sampler_state
{
	Texture = <test3>;
	MinFilter = LINEAR;  
	MagFilter = LINEAR;
	MipFilter = None;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = 0xFFFFFF;
};
sampler2D test4Sampler = sampler_state
{
	Texture = <test4>;
	MinFilter = LINEAR;  
	MagFilter = LINEAR;
	MipFilter = None;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = 0xFFFFFF;
};
samplerCUBE cubemapSampler = sampler_state
{
   Texture = <cubemap>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Wrap;
   AddressV = Wrap;
};
struct VS_OUTPUT_POST {
	float4 vpos       : POSITION;
	float2 texcoord   : TEXCOORD0;
	float3 ViewRay	  : TEXCOORD1;
};
struct VS_INPUT_POST {
	float4 pos      : POSITION;
	float2 texcoord : TEXCOORD0;
};

VS_OUTPUT_POST mainVS(VS_INPUT_POST IN) {
	VS_OUTPUT_POST OUT;
	float4 Po = float4(IN.pos.xyz,1.0);
	OUT.vpos=Po;
	OUT.texcoord = CalculateVPos(OUT.vpos);
	float3 positionWS = mul(Po, gmViewProjInv).xyz;
	OUT.ViewRay = IN.pos.xyz;
	return OUT;
}

float4x4 make_bias_mat(float BiasVal)
{
	float fTexWidth = ShadowParams.x;
	float fTexHeight = ShadowParams.x;
	// float fZScale = pow(2.0,((float)SHAD_BIT_DEPTH))-1.0; // dx8
	float fZScale = 1.0; //dx9
	float fOffsetX = 0.5f + (0.5f / fTexWidth);
	float fOffsetY = 0.5f + (0.5f / fTexHeight);
	float4x4 result = float4x4(0.5f,     0.0f,     0.0f,      0.0f,
							   0.0f,    -0.5f,     0.0f,      0.0f,
							   0.0f,     0.0f,     fZScale,   0.0f,
							   fOffsetX, fOffsetY, -BiasVal,     1.0f );
	return result;
}
float4 g_fSplitDistances;
half GetSplitByDepth(float fDepth)
{
	float2 fTest = fDepth > g_fSplitDistances;
	return dot(fTest, fTest);
}
float3 offset_lookup(sampler2D map,
                     float4 loc,
                     float2 offset)
{
	float2 texmapscale = {ShadowParams.y,ShadowParams.y};
	return tex2Dproj(map, float4(loc.xy + offset * texmapscale * loc.w, loc.z, loc.w));
}
float sampleShadow(sampler2D shadowmap,float4 shadowCoord){
	float sum = 0;
	int x, y;

	for (y = 0; y <= 1; y++)
	  for (x = 0; x <= 1; x++)
		sum += offset_lookup(shadowmap, shadowCoord, float2(x, y));

	return sum / 4.0;
}

float GetShadow(half fSplitIndex, float4 faSplitUV[CascadeCount])
{
	float fShadow;
	if(fSplitIndex>=3) {
		fShadow = tex2Dproj(test4Sampler, faSplitUV[fSplitIndex]);
	} else if(fSplitIndex>=2) {
		fShadow = tex2Dproj(test3Sampler, faSplitUV[fSplitIndex]);
	} else if(fSplitIndex>=1) {
		fShadow = tex2Dproj(test2Sampler, faSplitUV[fSplitIndex]);
	} else {
		fShadow = sampleShadow(testSampler, faSplitUV[fSplitIndex]);
	}
	return fShadow;
}
float PhongSpecular(float3 normal, float3 viewDir, float specularDecay,float3 LightDir)
{
    float nDotL = dot(normal, LightDir);
    float3 reflection = 2.0f * normal * nDotL + -LightDir;
    reflection = normalize(reflection);
    float rdotV = saturate(dot(reflection, viewDir));
    return pow(rdotV, specularDecay);
}
float4 mainPS(VS_OUTPUT_POST IN) : COLOR {
	float4 normalSpec = tex2D(nsGBUFF,IN.texcoord);
	float3 normal = normalSpec.xyz;
	half  spec = normalSpec.w;
	float4 WSpos = tex2D(shadGBUFF,IN.texcoord);
	float4x4 bias_mat = make_bias_mat(ShadowParams.z);
	
	float4 shadow[CascadeCount];
	for(int n = 0; n < CascadeCount; n++ ){
		shadow[n] = mul(gmLightViewProj[n],float4(WSpos.xyz,1.0));
	}
	shadow[0] = mul(shadow[0],bias_mat);
	half fSplitIndex = GetSplitByDepth(WSpos.w);
	
	half lighting = (dot(normalize(lightDirection-WSpos.xyz),normal)).x*GetShadow(fSplitIndex, shadow)*SunColor;
    float dDepth;
	return float4((lighting.xxx),0.0f);
}
float4 mainPL_PS(VS_OUTPUT_POST IN) : COLOR {
	float4 normalSpec = tex2D(nsGBUFF,IN.texcoord);
	float3 normal = normalSpec.xyz;
	float3 WSpos = tex2D(shadGBUFF,IN.texcoord).xyz;
	float3 lightVec = (sLP-WSpos.xyz);
	float lengt = length(lightVec);
	lightVec = lightVec / lengt;
	half atten = max(1.0f - (lengt / PointLightRange), 0.0f);
	half3 r    = reflect ( normalize(gmViewInv[3].xyz-WSpos.xyz), normal );
	float spec = PhongSpecular(normal,normalize( gmViewInv[3].xyz-WSpos.xyz),256,lightVec)*atten;
	half lighting = max(dot(lightVec,normal),0.0).x;
	return float4(saturate(lighting.xxx*atten*PointLightColor),spec);
}
float4 mainL_PS(VS_OUTPUT_POST IN) : COLOR {
	float4 color = tex2D(colorGBUFF,IN.texcoord);
	float4 normalSpec = tex2D(nsGBUFF,IN.texcoord);
	float3 WSpos = tex2D(shadGBUFF,IN.texcoord).xyz;
	float3 normal = normalSpec.xyz;
	float3 lighting = tex2D(lightBUFF,IN.texcoord).xyz+(0.3*gvAmbientColor2.xyz);
	float3 refvect = reflect(normalize(gmViewInv[3].xyz-WSpos.xyz),normal);
	float4 reflColor = texCUBE(cubemapSampler,-refvect.xyz);
	if(color.w > 1){
		return float4(color.xyz,1);
	} else {
		return float4(lighting*color.xyz+(reflColor.xyz*normalSpec.w*color.xyz)+(tex2D(lightBUFF,IN.texcoord).w*normalSpec.w),1);
	}
}

technique DefShad {
	pass p0 {
		VertexShader = compile vs_2_0 mainVS();
		PixelShader  = compile ps_2_0 mainPS();
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		ZEnable = false;
		ZWriteEnable = false;
	}
};
technique DefShadPL {
	pass p0 {
		VertexShader = compile vs_2_0 mainVS();
		PixelShader  = compile ps_2_0 mainPL_PS();
		AlphaTestEnable = false;
		AlphaBlendEnable = true;
		ZEnable = false;
		ZWriteEnable = false;
		ScissorTestEnable = true;
		SrcBlend = One;
		DestBlend = One;
	}
};
technique DefShadL {
	pass p0 {
		VertexShader = compile vs_2_0 mainVS();
		PixelShader  = compile ps_2_0 mainL_PS();
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		ZEnable = false;
		ZWriteEnable = false;
	}
};