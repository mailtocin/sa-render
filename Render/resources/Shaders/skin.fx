float4x4 gmWorldViewProj;
float4x4 gmWorld;
float4x4 gmWorldView;
float4x4 TexTransform;
float4x4 gmLightViewProj;
float4 gvDirLight;
float4 gvColor = float4(1,1,1,1);
float4 gvAmbientColor;
float4 gvAmbientColor2;
float4 gvEye;

const float4 constdata[224];
float screenHeight;
float screenWidth;
float gfSpecularFactor = 0;
texture2D shadowTex;
texture2D gtDiffuse;
texture2D gtNormals;
texture2D gtSpecular;
texture2D gtNoise;
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

struct VS_OUTPUT
{
    float4 vpos     : POSITION;
    float3 texcoord : TEXCOORD0;
    float3 normal   : TEXCOORD1;
	float4 depth 	: TEXCOORD2;
	float3 tangent  : TEXCOORD3;
	float3 binormal : TEXCOORD4;
};

struct VS_SHADOW_OUTPUT
{
    float4 vpos     : POSITION;
	float depth 	: TEXCOORD1;
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

struct Deferred_OUT
{
    float4 col0      : COLOR0;
    float4 col1 	 : COLOR1;
	float4 col2 	 : COLOR2;
	//float4 col3 	 : COLOR3;
};

VS_OUTPUT DeferredVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
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
	OUT.vpos=mul(gmWorldViewProj,OUT.vpos);
    OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
	OUT.depth.xyz = wpos;
	OUT.depth.w = 1;
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
	OUT.depth = OUT.vpos.z;
	OUT.texcoord = IN.texcoord;
    return OUT;
}
float4 shadowPS(VS_SHADOW_OUTPUT IN) : COLOR
{
	float4 texColor = tex2D(gsDiffuse, IN.texcoord.xy);
	clip(texColor.a);
	return float4(IN.depth,0,0,texColor.a);
}
float2 fInverseViewportDimensions = {1.0/1024.0,1.0/768.0};
Deferred_OUT DeferredPS(VS_OUTPUT IN,float2 viewpos:VPOS)
{
	Deferred_OUT OUT;
	float3 vNormal = (tex2D( gsNormals, IN.texcoord.xy ));
	vNormal = 2 * vNormal - 1.0;
	vNormal.z = 1;
	float3x3 mTangentToWorld = transpose( float3x3( IN.tangent, IN.binormal, IN.normal ) );
	float3   vNormalWorld    = normalize( mul( mTangentToWorld, vNormal ));
	float2 tc = viewpos*fInverseViewportDimensions + fInverseViewportDimensions*0.5f;
	OUT.col0 = tex2D(gsDiffuse, IN.texcoord.xy)* gvColor;
	OUT.col1.xyz = vNormalWorld.xyz;
	float spec = (tex2D( gsSpecular, IN.texcoord.xy ).x>0)? tex2D( gsSpecular, IN.texcoord.xy ).x*1.0 : 0.0;
	OUT.col1.w = spec;
	OUT.col2 = float4(IN.depth.xyz,IN.texcoord.z);
	float clipingAlpha = 1-tex2D(gsNoise,1024.0f*tc/64.0f).x;
	OUT.col0.w = (OUT.col0.w<0.99f&&OUT.col0.w>0.01f)? clipingAlpha*OUT.col0.w:OUT.col0.w;
	clip(OUT.col0.w);
	//OUT.col3 = float4(1,1,1,1);
	return OUT;
}

technique Deferred
{
    pass p0
    {
        VertexShader = compile vs_3_0 DeferredVS();
        PixelShader  = compile ps_3_0 DeferredPS();
		AlphaTestEnable = true;
		AlphaBlendEnable = false;
    }
};
technique Shadow
{
    pass p0
    {
        VertexShader = compile vs_3_0 shadowVS();
        PixelShader  = compile ps_3_0 shadowPS();
		AlphaTestEnable = true;
		AlphaBlendEnable = false;
    }
};