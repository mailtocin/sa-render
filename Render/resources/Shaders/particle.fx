#include "Helpers.fx"
//--------------------------------Matrix's--------------------------------
float4x4 gmViewProj;
float4x4 gmInvProj;
//--------------------------------Textures--------------------------------
texture2D gtDiffuse;
texture2D gtDepth;
//--------------------------------Vectors---------------------------------
float3 g_LightDir ={0,0,1};
float3 g_vRight;
float3 g_vUp;
float3 g_vForward;
float3 gvAmbientColor;
float3 gvAmbientColor2;
//--------------------------------Samplers--------------------------------
sampler2D gsDiffuse = sampler_state
{
   Texture = <gtDiffuse>;
	MinFilter = LINEAR;  
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = Wrap;
    AddressV  = Wrap;
};
sampler2D gsDepth = sampler_state
{
   Texture = <gtDepth>;
	MinFilter = LINEAR;  
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = Wrap;
    AddressV  = Wrap;
};
//--------------------------------IN/OUT structures-----------------------
struct VS_OUTPUT
{
    float4 vpos     : POSITION;
    float3 texcoord : TEXCOORD0;
	float4 color    : TEXCOORD1;
	float3 wpos     : TEXCOORD2;
	float2 texcoord1: TEXCOORD3;
};
struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 texcoord : TEXCOORD0;
	float4 color    : COLOR;
};
//--------------------------------Vertex Shaders--------------------------
VS_OUTPUT ForwardVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.vpos  = mul(gmViewProj,float4(IN.pos.xyz,1.0));
	OUT.color = IN.color;
    OUT.texcoord = float3(IN.texcoord.xy,OUT.vpos.z);
	OUT.wpos = IN.pos.xyz;
	OUT.texcoord1 = CalculateVPos(OUT.vpos);
    return OUT;
}
//--------------------------------Pixel Shaders---------------------------
float4 ForwardPS(VS_OUTPUT IN) : COLOR0
{
	float4 texColor = tex2D(gsDiffuse, IN.texcoord.xy);
	float3 norm = AutoNormalGen(gsDiffuse, IN.texcoord.xy, 8) * 2 - 1;
	norm.xy *= 3;
	float3 worldnorm;
	worldnorm = -norm.x * g_vRight;
    worldnorm += norm.y * g_vUp;
    worldnorm += -norm.z * g_vForward;
	float3 lighting = max(dot(normalize(worldnorm),-normalize(g_LightDir-IN.wpos)),0.0f)+gvAmbientColor2*0.5f;
	float depthFade = 1;
	float depth = tex2D( gsDepth, IN.texcoord1 ).w;
	float2 viewpos = GetUnsizedVPos( IN.texcoord1 );
	float4 depthViewSample = float4( viewpos, depth, 1 );
    float4 depthViewParticle = float4( viewpos, IN.texcoord.z, 1 );
	float depthDiff = depthViewSample.z/depthViewSample.w - depthViewParticle.z/depthViewParticle.w;
	depthFade = saturate( depthDiff / 1.2f );
	float lum = dot(texColor.xyz,float3(0.22, 0.707, 0.071));
	texColor.w*=(texColor.w>=0.9)?lum:1;
	texColor.w*=depthFade;
	return float4(texColor.xyz*lighting*IN.color.xyz,texColor.w*IN.color.w);
}
//--------------------------------Techniques------------------------------
technique Forward
{
    pass p0
    {
        VertexShader = compile vs_2_0 ForwardVS();
        PixelShader  = compile ps_2_0 ForwardPS();
		AlphaTestEnable = false;
		AlphaBlendEnable = true;
		ZEnable = true;
		ZWriteEnable = false;
    }
};