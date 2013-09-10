//--------------------------------Matrix's--------------------------------
float4x4 gmWorldViewProj;
float4x4 gmWorld;
//--------------------------------Textures--------------------------------
const texture cloudTex;
const texture starsTex;
const texture starsMaskTex;
//--------------------------------Vectors---------------------------------
float3 lightDirection;
float3 skyColorTop;
float3 skyColorBottom;
float4 TopCloudColor;
//--------------------------------Scalars---------------------------------
float fCloudCover;
float fCloud1Transp;
float fRadius;
float time;
float StarsAlp = 0;
//--------------------------------Samplers--------------------------------
sampler2D cloudSampler = sampler_state
{
   Texture = <cloudTex>;
   MinFilter = LINEAR;
   MagFilter = LINEAR;
   MipFilter = None;
   AddressU = Wrap;
   AddressV = Wrap;
};
sampler StarsSample = sampler_state {
    Texture   = <starsTex>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler MaskSample = sampler_state {
    Texture   = <starsMaskTex>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
//--------------------------------Helpers---------------------------------
float4 ComputeClouds(float2 uv)
{
	float4 clTex = tex2D( cloudSampler, uv );
	// blend 4 channel of the cloud texture according to cloud cover 
	float4 vDensity;
	vDensity = abs( fCloudCover - float4( 0.25f, 0.5f, 0.75f, 1.0f ) ) / 0.25f;
	vDensity = saturate( 1.0f - vDensity );
	float _fDensity = dot( clTex, vDensity );
	return float4(_fDensity,_fDensity,_fDensity,_fDensity);
}
//--------------------------------IN/OUT structures-----------------------
struct Deferred_OUT
{
    float4 col0      : COLOR0;
    float4 col1 	 : COLOR1;
	float4 col2 	 : COLOR2;
};
struct VS_SKY_INPUT
{
    float4 pos      : POSITION;
	float3 normal   : NORMAL;
	float2 tex      : TEXCOORD0;
};
struct VS_SKY_OUTPUT
{
    float4 vpos     : POSITION;
	float3 normal   : TEXCOORD0;
	float3 normal2  : TEXCOORD1;
	float2 tex      : TEXCOORD2;
};
//--------------------------------Vertex Shaders--------------------------
VS_SKY_OUTPUT skyVS(VS_SKY_INPUT IN)
{
    VS_SKY_OUTPUT OUT;
	IN.pos *= fRadius*0.9;
	float3 wpos = mul(IN.pos,gmWorld).xyz;
    OUT.vpos=mul(gmWorldViewProj,float4(IN.pos.xyz,1.0));
	OUT.normal = IN.pos.xyz;
	OUT.tex.xy=IN.tex.xy;
	OUT.normal2=IN.normal;
    return OUT;
}
//--------------------------------Pixel Shaders---------------------------
Deferred_OUT skyPS(VS_SKY_OUTPUT IN) {
	Deferred_OUT OUT;
	float3 skyColor = lerp(skyColorBottom,skyColorTop,smoothstep(-fRadius*2.5f, fRadius*2.5f, IN.normal.z));
	if ( StarsAlp > 0 ) {
		float4 Stars = tex2D( StarsSample, IN.tex * 4 ) ;
		Stars *= tex2D( MaskSample, ( IN.tex * 2 ) + ( time / 75 ) ) * StarsAlp;
		
		skyColor += lerp(0 , Stars * 5, saturate((IN.normal.z - (-fRadius*2.5f)) / ((fRadius*2.5f) - (-fRadius*2.5f))));
	}
	OUT.col0 = float4(skyColor,2.0);
	OUT.col1 = float4(0,0,1,0);
	OUT.col2 = float4(0,0,0,3500);
	return OUT;
}
//--------------------------------Techniques------------------------------
technique Sky {
	pass p0 {
		VertexShader = compile vs_2_0 skyVS();
		PixelShader  = compile ps_2_0 skyPS();
		AlphaTestEnable = true;
		AlphaBlendEnable = false;
	}
};