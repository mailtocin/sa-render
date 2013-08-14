//#include "helpers.fxh"
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
float4x4 gmLightViewProj[4];
float4 SunColor;
float4 gvAmbientColor;
float4 gvAmbientColor2;
float PointLightRange;
float ScreenSizeX;
float ScreenSizeY;
float3 PointLightColor;
int PostProcessCount = 8;
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
sampler2D normalscreenSampler = sampler_state
{
   Texture = <screenBuffer_0>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D ssaoSampler = sampler_state
{
   Texture = <screenBuffer_1>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D blurredByY_SSAO_Sampler = sampler_state
{
   Texture = <screenBuffer_2>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D blurredSSAO_Sampler = sampler_state
{
   Texture = <screenBuffer_3>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D combinedSSAO_Sampler = sampler_state
{
   Texture = <screenBuffer_4>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D tresholded_Bloom_Sampler = sampler_state
{
   Texture = <screenBuffer_5>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D blurredByY_Bloom_Sampler = sampler_state
{
   Texture = <screenBuffer_6>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D blurred_Bloom_Sampler = sampler_state
{
   Texture = <screenBuffer_7>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D combined_Bloom_Sampler = sampler_state
{
   Texture = <screenBuffer_8>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D log_Tonemap_Sampler = sampler_state
{
   Texture = <screenBuffer_9>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D adapted_Tonemap_Sampler = sampler_state
{
   Texture = <screenBuffer_10>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D small_Tonemap_Sampler = sampler_state
{
   Texture = <screenBuffer_11>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
sampler2D out_Tonemap_Sampler = sampler_state
{
   Texture = <screenBuffer_12>;
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
	MinFilter = LINEAR;  
	MagFilter = LINEAR;
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
	OUT.texcoord = IN.texcoord;
	float3 positionWS = mul(Po, gmViewProjInv).xyz;
	OUT.ViewRay = gmViewInv[2].xyz;
	return OUT;
}

float4x4 make_bias_mat(float BiasVal)
{
	float fTexWidth = ScreenSizeX;
	float fTexHeight = ScreenSizeY;
	// float fZScale = pow(2.0,((float)SHAD_BIT_DEPTH))-1.0; // dx8
	float fZScale = 1.0; //dx9
	float fOffsetX = 0.5f + (0.5f / fTexWidth);
	float fOffsetY = 0.5f + (0.5f / fTexHeight);
	float4x4 result = float4x4(0.5f,     0.0f,     0.0f,      0.0f,
							   0.0f,    -0.5f,     0.0f,      0.0f,
							   0.0f,     0.0f,     fZScale,   0.0f,
							   fOffsetX, fOffsetY, BiasVal,     1.0f );
	return result;
}
float4 g_fSplitDistances;
half GetSplitByDepth(float fDepth)
{
	float2 fTest = fDepth > g_fSplitDistances;
	return dot(fTest, fTest);
}
float GetShadow(half fSplitIndex, float4 faSplitUV[4])
{
	float fShadow;
	if(fSplitIndex>=3) {
		fShadow = tex2Dproj(test4Sampler, faSplitUV[fSplitIndex]);
	} else if(fSplitIndex>=2) {
		fShadow = tex2Dproj(test3Sampler, faSplitUV[fSplitIndex]);
	} else if(fSplitIndex>=1) {
		fShadow = tex2Dproj(test2Sampler, faSplitUV[fSplitIndex]);
	} else {
		fShadow = tex2Dproj(testSampler, faSplitUV[fSplitIndex]);
	}
	return fShadow;
}
float4 mainPS(VS_OUTPUT_POST IN) : COLOR {
	float4 normalSpec = tex2D(nsGBUFF,IN.texcoord);
	float3 normal = normalSpec.xyz;
	half  spec = normalSpec.w;
	float4 WSpos = tex2D(shadGBUFF,IN.texcoord);
	float4x4 bias_mat = make_bias_mat(0.0001f);
	
	float4 shadow[4];
	for(int n = 0; n < 4; n++ ){
		shadow[n] = mul(mul(gmLightViewProj[n],float4(WSpos.xyz,1.0)),bias_mat);
	}
	half fSplitIndex = GetSplitByDepth(WSpos.w);
	
	half lighting = saturate(dot(normalize(lightDirection-WSpos.xyz),normal)).x*GetShadow(fSplitIndex, shadow);
	//float3 ambient = lerp(gvAmbientColor2.xyz,gvAmbientColor.xyz,normalize(1-sqrt(1-dot(normal.xy, normal.xy))));
	//float3 finalColor = (saturate(lighting)*SunColor+0.3)*tex2D(colorGBUFF,IN.texcoord).xyz;
	//float  fNdotV     = saturate(dot( normal, normalize(gmViewInv[2].xyz)));
	//float3 vReflection = 2 * normal * fNdotV - normalize(gmViewInv[2].xyz);
	//float3 hitPos = viewPos;
    float dDepth;
	return float4(saturate(lighting.xxx),1);
}
float4 mainPL_PS(VS_OUTPUT_POST IN) : COLOR {
	float4 normalSpec = tex2D(nsGBUFF,IN.texcoord);
	float3 normal = normalSpec.xyz;
	float3 WSpos = tex2D(shadGBUFF,IN.texcoord).xyz;
	float3 lightVec = (sLP-WSpos.xyz);
	float lengt = length(lightVec);
	lightVec = lightVec / lengt;
	half atten = max(1.0f - (lengt / PointLightRange), 0.0f);
	half3 r    = reflect ( -normalize(IN.ViewRay), normal );
	float spec = pow(max(0,dot(lightVec,r)),65.0);
	half lighting = max(dot(lightVec,normal),0.0).x;
	return float4(saturate(lighting.xxx*atten*PointLightColor),spec);
}
float4 mainL_PS(VS_OUTPUT_POST IN) : COLOR {
	float4 normalSpec = tex2D(nsGBUFF,IN.texcoord);
	float d = tex2D(colorGBUFF,IN.texcoord).w;
	float3 normal = normalSpec.xyz;
	float3 ambient = lerp(gvAmbientColor2.xyz,gvAmbientColor.xyz,(normal.z));
	float3 lighting = tex2D(lightBUFF,IN.texcoord).xyz+(0.3*ambient);
	float3 refvect = reflect(normalize(IN.ViewRay),normal);
	float4 reflColor = texCUBE(cubemapSampler,refvect.xyz);
	if(d > 1){
		return float4(tex2D(colorGBUFF,IN.texcoord).xyz,1);
	} else {
		return float4(lighting*tex2D(colorGBUFF,IN.texcoord).xyz+(reflColor.xyz*normalSpec.w*tex2D(colorGBUFF,IN.texcoord).xyz),1);
	}
}
float4 PP2_PS(VS_OUTPUT_POST IN) : COLOR {
	
	float totStrength = 1.38;
	float strength = 0.07;
	float offset = 18.0;
	float falloff = 0.000002;
	float rad = 0.3;
	const float invSamples = 1.0/16.0;
	//float3 pSphere[16] = {float3(0.53812504, 0.18565957, -0.43192),float3(0.13790712, 0.24864247, 0.44301823),float3(0.33715037, 0.56794053, -0.005789503),float3(-0.6999805, -0.04511441, -0.0019965635),float3(0.06896307, -0.15983082, -0.85477847),float3(0.056099437, 0.006954967, -0.1843352),float3(-0.014653638, 0.14027752, 0.0762037),float3(0.010019933, -0.1924225, -0.034443386),float3(-0.35775623, -0.5301969, -0.43581226),float3(-0.3169221, 0.106360726, 0.015860917),float3(0.010350345, -0.58698344, 0.0046293875),float3(-0.08972908, -0.49408212, 0.3287904),float3(0.7119986, -0.0154690035, -0.09183723),float3(-0.053382345, 0.059675813, -0.5411899),float3(0.035267662, -0.063188605, 0.54602677),float3(-0.47761092, 0.2847911, -0.0271716)};
	//const float3 pSphere[8] = {float3(0.24710192, 0.6445882, 0.033550154),float3(0.00991752, -0.21947019, 0.7196721),float3(0.25109035, -0.1787317, -0.011580509),float3(-0.08781511, 0.44514698, 0.56647956),float3(-0.011737816, -0.0643377, 0.16030222),float3(0.035941467, 0.04990871, -0.46533614),float3(-0.058801126, 0.7347013, -0.25399926),float3(-0.24799341, -0.022052078, -0.13399573)};
	//const float3 pSphere[12] = float3[](float3(-0.13657719, 0.30651027, 0.16118456),float3(-0.14714938, 0.33245975, -0.113095455),float3(0.030659059, 0.27887347, -0.7332209),float3(0.009913514, -0.89884496, 0.07381549),float3(0.040318526, 0.40091, 0.6847858),float3(0.22311053, -0.3039437, -0.19340435),float3(0.36235332, 0.21894878, -0.05407306),float3(-0.15198798, -0.38409665, -0.46785462),float3(-0.013492276, -0.5345803, 0.11307949),float3(-0.4972847, 0.037064247, -0.4381323),float3(-0.024175806, -0.008928787, 0.17719103),float3(0.694014, -0.122672155, 0.33098832));
	//const float3 pSphere[10] = float3[](float3(-0.010735935, 0.01647018, 0.0062425877),float3(-0.06533369, 0.3647007, -0.13746321),float3(-0.6539235, -0.016726388, -0.53000957),float3(0.40958285, 0.0052428036, -0.5591124),float3(-0.1465366, 0.09899267, 0.15571679),float3(-0.44122112, -0.5458797, 0.04912532),float3(0.03755566, -0.10961345, -0.33040273),float3(0.019100213, 0.29652783, 0.066237666),float3(0.8765323, 0.011236004, 0.28265962),float3(0.29264435, -0.40794238, 0.15964167));
	const float3 pSphere[16] = {
	  float3(-0.4751779,-7.450581E-05,-0.9013236),
	  float3(0.9366915,0.5170922,0.6566324),
	  float3(0.9670465,-0.8144083,0.6265306),
	  float3(0.6308529,0.106269,-0.7456973),
	  float3(-0.1644763,0.9550674,-0.8187573),
	  float3(-0.5530901,0.8803544,0.9971306),
	  float3(0.3727033,0.975286,0.527049),
	  float3(0.9366906,-0.174755,0.7764492),
	  float3(-0.7752252,-0.6397214,0.5226125),
	  float3(-0.6470571,-0.1127397,0.2543334),
	  float3(0.4471788,-0.1979314,0.1179909),
	  float3(0.6223125,-0.7352273,0.6094033),
	  float3(-0.8523862,0.8261747,-0.6282346),
	  float3(0.05481017,0.4686372,0.05818021),
	  float3(-0.560359,0.2093774,-0.7031121),
	  float3(0.8116179,0.7803779,-0.9776397),
	};
	float3 fres = normalize((tex2D(noiseSampler,512*IN.texcoord/64).xyz*2.0) - 1.0);
	float3 normal = tex2D(nsGBUFF,IN.texcoord).xyz;
	float currentPixelDepth = tex2D(shadGBUFF,IN.texcoord).w;
	float3 ep = float3(IN.texcoord.xy,currentPixelDepth);
	
	float bl = 0.0;
	float radD = rad/currentPixelDepth;
 
	float3 ray, se, occNorm,bounce,outBounce=0,occluderFragment;
	float occluderDepth, depthDifference, normDiff;
 
	for(int i=0; i<16;i++)
	{
	  // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
	  ray = reflect(pSphere[i],fres);
	  //half flip = (dot(normal,ray)<0) ? 1.0 : -1.0;
      //ray *= -flip;
	  //ray += normal * 0.3;
	  float2 offset = ray.xy * radD;
	  float sD = currentPixelDepth - (ray.z * rad);
	  
	  occluderFragment = tex2D(nsGBUFF,ep.xy + offset).xyz;
	  // if depthDifference is negative = occluder is behind current fragment
	  depthDifference = saturate(currentPixelDepth-(tex2D(shadGBUFF,ep.xy + offset).w));
	  
	  bounce = tex2D(colorGBUFF,ep.xy + offset).xyz;
	  if (depthDifference > 0.01) {
		// the falloff equation, starts at falloff and is kind of 1/x^2 falling
		bl += pow(1-depthDifference,1.0);
		outBounce += pow(bounce,1.0);
	  }
	}
 
	// output the result
	float ao = saturate(1-(bl/16)+0.5);
	outBounce /= 16;
	return float4(ao.xxx,1);
}
float4 GuassianBlurVertPS(VS_OUTPUT_POST IN,uniform sampler2D sample,uniform int texSize) : COLOR {
	float3 outColor = 0.0;
	float blurSize = 1.0/(float)texSize;
	outColor += tex2D(sample, float2(IN.texcoord.x - 4.0*blurSize, IN.texcoord.y)) * 0.05;
	outColor += tex2D(sample, float2(IN.texcoord.x - 3.0*blurSize, IN.texcoord.y)) * 0.09;
	outColor += tex2D(sample, float2(IN.texcoord.x - 2.0*blurSize, IN.texcoord.y)) * 0.12;
	outColor += tex2D(sample, float2(IN.texcoord.x - blurSize, IN.texcoord.y)) * 0.15;
	outColor += tex2D(sample, float2(IN.texcoord.x, IN.texcoord.y)) * 0.16;
	outColor += tex2D(sample, float2(IN.texcoord.x + blurSize, IN.texcoord.y)) * 0.15;
	outColor += tex2D(sample, float2(IN.texcoord.x + 2.0*blurSize, IN.texcoord.y)) * 0.12;
	outColor += tex2D(sample, float2(IN.texcoord.x + 3.0*blurSize, IN.texcoord.y)) * 0.09;
	outColor += tex2D(sample, float2(IN.texcoord.x + 4.0*blurSize, IN.texcoord.y)) * 0.05;
	return float4(outColor,1);
}
float4 GuassianBlurHorizPS(VS_OUTPUT_POST IN,uniform sampler2D sample,uniform int texSize) : COLOR {
	float3 outColor = 0.0;
	float blurSize = 1.0/(float)texSize;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y - 4.0*blurSize)) * 0.05;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y - 3.0*blurSize)) * 0.09;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y - 2.0*blurSize)) * 0.12;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y - blurSize)) * 0.15;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y)) * 0.16;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y + blurSize)) * 0.15;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y + 2.0*blurSize)) * 0.12;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y + 3.0*blurSize)) * 0.09;
	outColor += tex2D(sample, float2(IN.texcoord.x,IN.texcoord.y + 4.0*blurSize)) * 0.05;
	return float4(outColor,1);
}
float4 SSAO_Combine_PassPS(VS_OUTPUT_POST IN) : COLOR {
	float3 outColor = tex2D(normalscreenSampler, IN.texcoord.xy).xyz*pow(tex2D(blurredSSAO_Sampler, IN.texcoord.xy).x,4.0);
	return float4(outColor,1);
}
float4 Bloom_treshold_PassPS(VS_OUTPUT_POST IN) : COLOR {
	float3 outColor = max(tex2D(normalscreenSampler, IN.texcoord.xy).xyz-0.5,0.0);
	return float4(outColor,1);
}
float4 Bloom_Combine_PassPS(VS_OUTPUT_POST IN) : COLOR {
	float3 outColor = tex2D(combinedSSAO_Sampler, IN.texcoord.xy).xyz+tex2D(blurred_Bloom_Sampler, IN.texcoord.xy).xyz;
	return float4(outColor,1);
}
inline float Luminance( float3 c )
{
	return dot( c, float3(0.22, 0.707, 0.071) );
}
float4 Tonemap_Log_PassPS(VS_OUTPUT_POST IN) : COLOR 
{
	const float DELTA = 0.0001f;

	float fLogLumSum = 0.0f;
	float2 _MainTex_TexelSize;
	_MainTex_TexelSize.x = 1.0/512.0;
	_MainTex_TexelSize.y = 1.0/512.0;

	fLogLumSum += log( Luminance(tex2D(combined_Bloom_Sampler, IN.texcoord + _MainTex_TexelSize.xy * float2(-1,-1)).rgb) + DELTA);		
	fLogLumSum += log( Luminance(tex2D(combined_Bloom_Sampler, IN.texcoord + _MainTex_TexelSize.xy * float2(1,1)).rgb) + DELTA);		
	fLogLumSum += log( Luminance(tex2D(combined_Bloom_Sampler, IN.texcoord + _MainTex_TexelSize.xy * float2(-1,1)).rgb) + DELTA);		
	fLogLumSum += log( Luminance(tex2D(combined_Bloom_Sampler, IN.texcoord + _MainTex_TexelSize.xy * float2(1,-1)).rgb) + DELTA);		

	float avg = fLogLumSum / 4.0;
	return float4(avg,avg,avg, avg);
}
float4 Blit_PassPS(VS_OUTPUT_POST IN,uniform sampler2D sample) : COLOR 
{
	return tex2D(sample,IN.texcoord);
}
float4 fragExp(VS_OUTPUT_POST IN) : COLOR 
{
	float2 lum = float2(0.0f, 0.0f);
	float2 _MainTex_TexelSize;
	_MainTex_TexelSize.x = 1.0/256.0;
	_MainTex_TexelSize.y = 1.0/256.0;
	lum += tex2D(adapted_Tonemap_Sampler, IN.texcoord  + _MainTex_TexelSize.xy * float2(-1,-1)).xy;	
	lum += tex2D(adapted_Tonemap_Sampler, IN.texcoord  + _MainTex_TexelSize.xy * float2(1,1)).xy;	
	lum += tex2D(adapted_Tonemap_Sampler, IN.texcoord + _MainTex_TexelSize.xy * float2(1,-1)).xy;	
	lum += tex2D(adapted_Tonemap_Sampler, IN.texcoord  + _MainTex_TexelSize.xy * float2(-1,1)).xy;	

	lum = exp(lum / 4.0f);
	
	return float4(lum.x, lum.y, lum.x, saturate(0.0125 * 1.5));
}
float4 fragAdaptive(VS_OUTPUT_POST IN) : COLOR 
{
	float avgLum = tex2D(small_Tonemap_Sampler, IN.texcoord).x;
	float4 color = tex2D (combined_Bloom_Sampler, IN.texcoord);
	
	float cieLum = max(0.000001, Luminance(color.rgb)); //ToCIE(color.rgb);
	
	float lumScaled = cieLum * 0.2 / (0.001 + avgLum);
	
	lumScaled = (lumScaled * (1.0f + lumScaled / (1.0)))/(1.0f + lumScaled);
	
	//cie.r = lumScaled; 
	
	color.rgb = color.rgb * (lumScaled / cieLum);
	
	//color.rgb = FromCIE(cie);		
	return color;
}

technique DefShad {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 mainPS();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique DefShadPL {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 mainPL_PS();
		AlphaTestEnable = false;
		AlphaBlendEnable = true;
		SrcBlend = ONE;
		DestBlend = ONE;
		CullMode		= None;
		SCISSORTESTENABLE = FALSE;
	}
};
technique DefShadL {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 mainL_PS();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_0 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 PP2_PS();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_1 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 GuassianBlurVertPS(ssaoSampler,512);
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_2 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 GuassianBlurHorizPS(blurredByY_SSAO_Sampler,512);
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_3 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 SSAO_Combine_PassPS();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_4 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 Bloom_treshold_PassPS();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_5 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 GuassianBlurVertPS(tresholded_Bloom_Sampler,256);
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_6 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 GuassianBlurHorizPS(blurredByY_Bloom_Sampler,256);
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_7 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 Bloom_Combine_PassPS();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_8 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 Tonemap_Log_PassPS();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_9 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 Blit_PassPS(log_Tonemap_Sampler);
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};
technique PostProcess_10 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 fragExp();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=TRUE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
	}
};
technique PostProcess_11 {
	pass p0 {
		VertexShader = compile vs_3_0 mainVS();
		PixelShader  = compile ps_3_0 fragAdaptive();
		ZEnable=FALSE;
		CullMode=NONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
	}
};