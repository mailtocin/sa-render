#include "Helpers.fx"
//--------------------------------Matrix's--------------------------------
float4x4 gmWorldViewProj;
float4x4 gmWorld;
//--------------------------------Textures--------------------------------
texture2D gtDiffuse;
texture2D gtNormals;
texture2D gtSpecular;
texture2D gtNoise;
//--------------------------------Vectors---------------------------------
float4 gvColor = float4(1,1,1,1);
const float4 constdata[224];
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
	float4 weight : BLENDWEIGHT;
	float4 bindex : BLENDINDICES;
};
//--------------------------------Vertex Shaders--------------------------
VS_DEFERRED_OUTPUT DeferredVS(VS_INPUT IN)
{
    VS_DEFERRED_OUTPUT OUT;
	float4 Po = float4(IN.pos.xyz,1.0);
	const float bsa = 7;
	if (IN.weight[0] != 0.0)
	{
		OUT.vpos = float4(0.0, 0.0, 0.0, 0.0);
		OUT.normal = float3(0.0, 0.0, 0.0);
		OUT.tangent = float3(0.0, 0.0, 0.0);
		for (int i = 0; i < 4; i++)
		{
			OUT.vpos += float4(dot(IN.pos, constdata[IN.bindex[i] + bsa - 5]), dot(IN.pos, constdata[IN.bindex[i] + bsa - 4]), dot(IN.pos, constdata[IN.bindex[i] + bsa - 3]), 1.0) * IN.weight[i];
			OUT.normal += float3(dot(IN.normal, constdata[IN.bindex[i] + bsa - 5]), dot(IN.normal, constdata[IN.bindex[i] + bsa - 4]), dot(IN.normal, constdata[IN.bindex[i] + bsa - 3])) * IN.weight[i];
			OUT.tangent += float3(dot(IN.tangent, constdata[IN.bindex[i] + bsa - 5]), dot(IN.tangent, constdata[IN.bindex[i] + bsa - 4]), dot(IN.tangent, constdata[IN.bindex[i] + bsa - 3])) * IN.weight[i];
		}
	} else
	{
		OUT.vpos = float4(dot(IN.pos, constdata[IN.bindex[0] + bsa - 5]), dot(IN.pos, constdata[IN.bindex[0] + bsa - 4]), dot(IN.pos, constdata[IN.bindex[0] + bsa - 3]), 1.0);
		OUT.normal = float3(dot(IN.normal, constdata[IN.bindex[0] + bsa - 5]), dot(IN.normal, constdata[IN.bindex[0] + bsa - 4]), dot(IN.normal, constdata[IN.bindex[0] + bsa - 3]));
		OUT.tangent = float3(dot(IN.tangent, constdata[IN.bindex[0] + bsa - 5]), dot(IN.tangent, constdata[IN.bindex[0] + bsa - 4]), dot(IN.tangent, constdata[IN.bindex[0] + bsa - 3]));
	}
	OUT.vpos.w = 1;
    float3 wpos = mul(gmWorld, OUT.vpos).xyz;
	OUT.binormal = mul(gmWorld,normalize(cross(OUT.tangent,OUT.normal)));
    OUT.normal  = mul(gmWorld, OUT.normal.xyz);
	OUT.tangent = mul(gmWorld, OUT.tangent.xyz);
	OUT.vpos = mul(gmWorldViewProj,OUT.vpos);
    OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
	OUT.color = V4ONE;
	OUT.pos = wpos.xyz;
	OUT.tex2 = CalculateVPos(OUT.vpos);
    return OUT;
}
VS_SHADOW_OUTPUT shadowVS(VS_INPUT IN)
{
    VS_SHADOW_OUTPUT OUT;
	float4 Po = float4(IN.pos.xyz,1.0);
	const float bsa = 7;
	if (IN.weight[0] != 0.0)
	{
		OUT.vpos = float4(0.0, 0.0, 0.0, 0.0);
		for (int i = 0; i < 4; i++)
		{
			OUT.vpos += float4(dot(IN.pos, constdata[IN.bindex[i] + bsa - 5]), dot(IN.pos, constdata[IN.bindex[i] + bsa - 4]), dot(IN.pos, constdata[IN.bindex[i] + bsa - 3]), 1.0) * IN.weight[i];
		}
	} else
	{
		OUT.vpos = float4(dot(IN.pos, constdata[IN.bindex[0] + bsa - 5]), dot(IN.pos, constdata[IN.bindex[0] + bsa - 4]), dot(IN.pos, constdata[IN.bindex[0] + bsa - 3]), 1.0);
	}
	OUT.vpos.w = 1;
    OUT.vpos=mul(gmWorldViewProj, OUT.vpos);
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
					gfSpecularFactor, false, gvColor, IN, OUT );
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