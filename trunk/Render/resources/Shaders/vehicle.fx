#include "Helpers.fx"
//--------------------------------Matrix's--------------------------------
float4x4 gmWorldViewProj;
float4x4 gmWorld;
//--------------------------------Textures--------------------------------
texture2D gtNoise;
texture2D gtDiffuse;
texture2D gtNormals;
texture2D gtSpecular;
//--------------------------------Vectors---------------------------------
float4 gvPaintColor = float4(1,1,1,1);
//--------------------------------Scalars---------------------------------
float gfSpecularFactor = 0;
//--------------------------------Samplers--------------------------------
sampler2D gsNoise = sampler_state
{
   Texture = <gtNoise>;
	MinFilter = POINT;  
    MagFilter = POINT;
    MipFilter = None;
    AddressU  = Wrap;
    AddressV  = Wrap;
};
sampler2D gsDiffuse = sampler_state
{
   Texture = <gtDiffuse>;
	MinFilter = ANISOTROPIC;  
    MagFilter = ANISOTROPIC;
    MipFilter = ANISOTROPIC;
    AddressU  = Wrap;
    AddressV  = Wrap;
};
sampler2D gsNormals = sampler_state
{
   Texture = <gtNormals>;
	MinFilter = ANISOTROPIC;  
    MagFilter = ANISOTROPIC;
    MipFilter = ANISOTROPIC;
    AddressU  = Wrap;
    AddressV  = Wrap;
};
sampler2D gsSpecular = sampler_state
{
   Texture = <gtSpecular>;
	MinFilter = ANISOTROPIC;  
    MagFilter = ANISOTROPIC;
    MipFilter = ANISOTROPIC;
    AddressU  = Wrap;
    AddressV  = Wrap;
};
//--------------------------------IN/OUT structures-----------------------
struct VS_SHADOW_OUTPUT
{
    float4 vpos     : POSITION;
	float2 texcoord : TEXCOORD0;
};
struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
};
//--------------------------------Vertex Shaders--------------------------
VS_DEFERRED_OUTPUT DeferredVS(VS_INPUT IN)
{
    VS_DEFERRED_OUTPUT OUT;
    float3 wpos = mul(gmWorld, IN.pos).xyz;
    OUT.vpos=mul(gmWorldViewProj,float4(IN.pos.xyz,1.0));
    OUT.normal = (mul(gmWorld, IN.normal.xyz));
    OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
	OUT.color = V4ONE;
	OUT.tangent = (mul(gmWorld, IN.tangent.xyz));
	OUT.binormal = (mul(gmWorld,(cross(IN.tangent,IN.normal))));
	OUT.pos = mul(gmWorld,float4(IN.pos.xyz,1.0)).xyz;
	OUT.tex2 = CalculateVPos(OUT.vpos);
    return OUT;
}
VS_SHADOW_OUTPUT shadowVS(VS_INPUT IN)
{
    VS_SHADOW_OUTPUT OUT;
    OUT.vpos=mul(gmWorldViewProj, float4(IN.pos.xyz,1.0));
	OUT.texcoord = IN.texcoord;
    return OUT;
}
//--------------------------------Pixel Shaders---------------------------
float4 shadowPS(VS_SHADOW_OUTPUT IN) : COLOR
{
	float texAlpha = tex2D(gsDiffuse, IN.texcoord).w;
	clip(texAlpha);
	return float4(0,0,0,texAlpha);
}
Deferred_OUT DeferredPS(VS_DEFERRED_OUTPUT IN)
{
	Deferred_OUT OUT;
	OUT.albedo = V4ZERO; OUT.normal = V4ZERO; OUT.position = V4ZERO;
	OutputDeferred( gsDiffuse, gsNormals, gsSpecular, gsNoise,
					gfSpecularFactor, false, gvPaintColor, IN, OUT );
	return OUT;
}
//--------------------------------Techniques------------------------------
technique Deferred
{
    pass p0
    {
        VertexShader = compile vs_2_0 DeferredVS();
        PixelShader  = compile ps_2_0 DeferredPS();
		AlphaTestEnable = true;
		AlphaBlendEnable = false;
    }
};
technique Shadow
{
    pass p0
    {
        VertexShader = compile vs_2_0 shadowVS();
        PixelShader  = compile ps_2_0 shadowPS();
		AlphaTestEnable = true;
		AlphaBlendEnable = false;
		ColorWriteEnable = false;
    }
};