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
//--------------------------------Scalars---------------------------------
float gfSpecularFactor = 0;
float DPRdirection = 1;
bool vcol = false;
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
//--------------------------------Helpers---------------------------------
float3 AutoNormalGen(sampler2D sample,float2 texCoord) {
   float off = 1.0 / 256;
   float4 lightness = float4(0.2,0.59,0.11,0);
   // Take all neighbor samples
   float4 s00 = tex2D(sample, texCoord + float2(-off, -off));
   float4 s01 = tex2D(sample, texCoord + float2( 0,   -off));
   float4 s02 = tex2D(sample, texCoord + float2( off, -off));

   float4 s10 = tex2D(sample, texCoord + float2(-off,  0));
   float4 s12 = tex2D(sample, texCoord + float2( off,  0));

   float4 s20 = tex2D(sample, texCoord + float2(-off,  off));
   float4 s21 = tex2D(sample, texCoord + float2( 0,    off));
   float4 s22 = tex2D(sample, texCoord + float2( off,  off));

   // Slope in X direction
   float4 sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
   // Slope in Y direction
   float4 sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

   // Weight the slope in all channels, we use grayscale as height
   float sx = dot(sobelX, lightness);
   float sy = dot(sobelY, lightness);

   // Compose the normal
   float3 normal = normalize(float3(sx, sy, 1));

   // Pack [-1, 1] into [0, 1]
   return normal * 0.5 + 0.5;
}
//--------------------------------IN/OUT structures-----------------------
struct VS_SHADOW_OUTPUT
{
    float4 vpos     : POSITION;
	float depth 	: TEXCOORD1;
	float2 texcoord : TEXCOORD0;
};
struct VS_REFL_OUTPUT
{
    float4 vpos     : POSITION;
	float3 texcoord : TEXCOORD0;
};
struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float4 color    : COLOR;
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
	OUT.color = IN.color;
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
	OUT.depth = mul(gmWorldViewProj, float4(IN.pos.xyz,1.0)).z;
	OUT.texcoord = IN.texcoord;
    return OUT;
}
VS_REFL_OUTPUT ReflectionVS(VS_INPUT IN)
{
    VS_REFL_OUTPUT OUT;
    OUT.vpos=mul(gmWorldViewProj, float4(IN.pos.xyz,1.0));
	OUT.vpos.z *=DPRdirection;
	float L = length( OUT.vpos.xyz );
	OUT.vpos = OUT.vpos / L;
	
	OUT.texcoord.z = OUT.vpos.z;
	OUT.vpos.z += 1;
	
	OUT.vpos.xy /= OUT.vpos.z;
	
	OUT.vpos.z = (L - 0.1) / (500 - 0.1);
	OUT.vpos.w = 1;
	
	OUT.texcoord.xy = IN.texcoord;
    return OUT;
}
//--------------------------------Pixel Shaders---------------------------
float4 ReflectionPS(VS_REFL_OUTPUT IN) : COLOR
{
	float4 texColor = tex2D(gsDiffuse, IN.texcoord.xy);
	clip(IN.texcoord.z);
	return texColor;
}
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
					gfSpecularFactor, vcol, gvColor, IN, OUT );
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
technique Reflection
{
    pass p0
    {
        VertexShader = compile vs_2_0 ReflectionVS();
        PixelShader  = compile ps_2_0 ReflectionPS();
		AlphaTestEnable = true;
		AlphaBlendEnable = false;
    }
};