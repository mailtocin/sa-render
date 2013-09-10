#include "Helpers.fx"
//--------------------------------Textures--------------------------------
const texture tScreen;
const texture tBlur;
const texture tNoise;
const texture tLens;
const texture tWPosDepth;
//--------------------------------Vectors---------------------------------
float2 vFocusPos = {0.5,0.5};
float4 vFocusParams = {1.5,200.0,20.0,20.0};
//--------------------------------Scalars---------------------------------
static int samples = 3;
static int rings = 3;
bool bAutofocus = true;
//--------------------------------Samplers--------------------------------
sampler TexScreen=sampler_state {
    Texture   = <tScreen>;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
    ADDRESSW  = CLAMP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler TexBlur=sampler_state {
    Texture   = <tBlur>;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
    ADDRESSW  = CLAMP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler TexNoise=sampler_state {
    Texture   = <tNoise>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    MAGFILTER = POINT;
    MINFILTER = POINT;
    MIPFILTER = NONE;
};
sampler TexLens=sampler_state {
    Texture   = <tLens>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    MinFilter = LINEAR;
	MagFilter = LINEAR;
    MIPFILTER = NONE;
};
sampler2D WPosDepthTex = sampler_state
{
   Texture = <tWPosDepth>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Clamp;
   AddressV = Clamp;
};
//--------------------------------Helpers---------------------------------
float3 LensBlur(const sampler2D Tex, float2 texCoord,float max_radius) {
    float3 outColor = tex2D(Tex, texCoord).xyz;
	float weight = 1.0;
	int ringsamples;
	float w = fInverseViewportDimensions.x*max_radius;
	float h = fInverseViewportDimensions.y*max_radius;
	[unroll(rings)]
	for (int i = 1; i <= rings; i++) {
		ringsamples = i * samples;
		[unroll(ringsamples)]
		for (int j = 0 ; j < ringsamples ; j++){
			float step = PI*2.0 / float(ringsamples);
			float pw = (cos(float(j)*step)*float(i));
			float ph = (sin(float(j)*step)*float(i));
			outColor += tex2D(Tex,texCoord + float2(pw*w,ph*h))*lerp(1.0,(float(i))/((float)rings),0.3f);
			weight += 1.0*lerp(1.0,(float(i))/((float)rings),0.3f);
		}
	}
    return outColor/weight;  
}
//--------------------------------IN/OUT structures-----------------------
struct VS_OUTPUT
{
    float4 vpos     : POSITION0;
    float2 texcoord : TEXCOORD0;
};
struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 texcoord : TEXCOORD0;
};
//--------------------------------Vertex Shaders--------------------------
VS_OUTPUT PostProcessVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.vpos=IN.pos;
	OUT.texcoord.xy = CalculateVPos(OUT.vpos);
    return OUT;
}
//--------------------------------Pixel Shaders---------------------------
float4 BlurPS(VS_OUTPUT IN) : COLOR0 {
	float nearBlurDepth,farBlurDepth;
	if(bAutofocus) {
		float focusDepth = tex2D(WPosDepthTex,vFocusPos).w;
		nearBlurDepth = max(focusDepth-vFocusParams.z,0.0);
		farBlurDepth = focusDepth+vFocusParams.w;
	} else {
		nearBlurDepth = vFocusParams.x;
		farBlurDepth = vFocusParams.y;
	}
	float normalizedDepth = linerarizeDepth(tex2D(WPosDepthTex,IN.texcoord.xy).w,nearBlurDepth,farBlurDepth);
	return float4(LensBlur(TexScreen,IN.texcoord.xy,2.0f*normalizedDepth),1.0f);
}
float4 DepthOfFieldPS(VS_OUTPUT IN) : COLOR0 {
	float nearBlurDepth,farBlurDepth;
	if(bAutofocus) {
		float focusDepth = tex2D(WPosDepthTex,vFocusPos).w;
		nearBlurDepth = max(focusDepth-vFocusParams.z,0.0);
		farBlurDepth = focusDepth+vFocusParams.w;
	} else {
		nearBlurDepth = vFocusParams.x;
		farBlurDepth = vFocusParams.y;
	}
	float normalizedDepth = linerarizeDepth(tex2D(WPosDepthTex,IN.texcoord.xy).w,nearBlurDepth,farBlurDepth);
	return lerp(tex2D(TexScreen,IN.texcoord.xy),tex2D(TexBlur,IN.texcoord.xy),normalizedDepth);
}
//--------------------------------Techniques------------------------------
technique DepthOfField
{
    pass p0
    {
        VertexShader = compile vs_2_0 PostProcessVS();
        PixelShader  = compile ps_2_0 DepthOfFieldPS();
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		ZEnable = false;
		ZWriteEnable = false;
    }
};
technique Blur
{
    pass p0
    {
        VertexShader = compile vs_2_0 PostProcessVS();
        PixelShader  = compile ps_3_0 BlurPS();
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		ZEnable = false;
		ZWriteEnable = false;
    }
};