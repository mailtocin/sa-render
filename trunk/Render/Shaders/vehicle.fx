float4x4 gmWorldViewProj;
float4x4 gmWorld;
float4 gvPaintColor = float4(1,1,1,1);
texture2D shadowTex;
texture2D gtDiffuse;
texture2D gtNormals;
texture2D gtSpecular;
sampler2D shadowSampler = sampler_state
{
   Texture = <shadowTex>;
	MinFilter = LINEAR;  
    MagFilter = LINEAR;
    MipFilter = None;
    AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = 0xFFFFFF;
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
    float3 tangent  : TEXCOORD2;
	float3 binormal : TEXCOORD3;
	float3 wpos		: TEXCOORD4;
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
    float3 wpos = mul(gmWorld, IN.pos).xyz;
    OUT.vpos=mul(gmWorldViewProj,float4(IN.pos.xyz,1.0));
    OUT.normal = mul(gmWorld, IN.normal.xyz);
    OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
	OUT.tangent = mul(gmWorld, IN.tangent.xyz);
	OUT.binormal = mul(gmWorld,normalize(cross(IN.tangent,IN.normal)));
	OUT.wpos =wpos;
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



float4 shadowPS(VS_SHADOW_OUTPUT IN) : COLOR
{
	float4 texColor = tex2D(gsDiffuse, IN.texcoord);
	clip(texColor.a);
	return float4(IN.depth,0,0,texColor.a);
}
float2 EncodeViewNormalStereo( float3 n )
{
	float kScale = 1.7777;
	float2 enc;
	enc = n.xy / (n.z+1);
	enc /= kScale;
	enc = enc*0.5+0.5;
	return enc;
}

Deferred_OUT DeferredPS(VS_OUTPUT IN,float2 viewpos:VPOS)
{
	Deferred_OUT OUT;
	float3 vNormal = (tex2D( gsNormals, IN.texcoord.xy ));
	vNormal = 2 * vNormal - 1.0;
	float3x3 mTangentToWorld = transpose( float3x3( IN.tangent, IN.binormal, IN.normal ) );
	float3   vNormalWorld    = normalize( mul( mTangentToWorld, vNormal ));
	OUT.col0 = tex2D(gsDiffuse, IN.texcoord.xy)* gvPaintColor;
	OUT.col1.xy = vNormalWorld.xy;
	OUT.col1.z = tex2D( gsSpecular, IN.texcoord.xy ).x;
	OUT.col1.w = 1;
	
	OUT.col2 = float4(IN.wpos.xyz,1);
	//OUT.col3 = float4(1,1,1,1);
	return OUT;
}

technique Deferred
{
    pass p0
    {
        VertexShader = compile vs_3_0 DeferredVS();
        PixelShader  = compile ps_3_0 DeferredPS();
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		ALPHATESTENABLE=TRUE;
		SrcBlend = one;
		DestBlend = zero;
    }
};
technique Shadow
{
    pass p0
    {
        VertexShader = compile vs_3_0 shadowVS();
        PixelShader  = compile ps_3_0 shadowPS();
		SEPARATEALPHABLENDENABLE=FALSE;
		AlphaBlendEnable=FALSE;
		ALPHATESTENABLE=TRUE;
		SrcBlend = one;
		DestBlend = zero;
    }
};