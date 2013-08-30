//--------------------------------Matrix's--------------------------------
float4x4 gmWorldViewProj;
float4x4 gmWorld;
//--------------------------------Textures--------------------------------
texture2D cloudTex;
//--------------------------------Vectors---------------------------------
float3 lightDirection;
float3 skyColorTop;
float3 skyColorBottom;
float4 TopCloudColor;
//--------------------------------Scalars---------------------------------
float fCloudCover;
float fCloud1Transp;
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
	half sunInfluence = dot( normalize(lightDirection.xyz),normalize( IN.normal.xyz ) )*0.25+0.75;
	sunInfluence *= saturate( normalize(lightDirection.z) * 4.0 ) * 0.666 + 0.333;
	sunInfluence = pow(saturate(sunInfluence),720.0f);
	float3 skyColor = lerp(skyColorBottom,skyColorTop,saturate(dot(IN.normal2.xyz,float3(0,0,4))));
	float4 clouds = ComputeClouds(IN.tex.xy*5);
	OUT.col0 = float4(skyColor,1)*(1-clouds*fCloud1Transp)+(clouds*TopCloudColor*fCloud1Transp);
	//OUT.col0.xyz *= 0.9;
	OUT.col0.w = 2.0;
	OUT.col1 = float4(0,0,1,0);
	OUT.col2 = float4(0,0,0,3500);
	return OUT;
}
//--------------------------------Techniques------------------------------
technique Sky {
	pass p0 {
		VertexShader = compile vs_3_0 skyVS();
		PixelShader  = compile ps_3_0 skyPS();
		cullmode = none;
		SrcBlend = ZERO;
		DestBlend = ONE;
		ALPHATESTENABLE=FALSE;
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		FogEnable=FALSE;
		SRGBWRITEENABLE=FALSE;
		ZEnable=TRUE;
	}
};