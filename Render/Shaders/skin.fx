float4x4 gmWorldViewProj;
float4x4 gmWorld;
float4x4 gmWorldView;
float4x4 TexTransform;
float4x4 gmLightViewProj;
float4 gvDirLight;
float4 gvPaintColor = float4(1,1,1,1);
float4 gvAmbientColor;
float4 gvAmbientColor2;
float4 gvEye;
const float4 constdata[224];
float screenHeight;
float screenWidth;
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
    float3 light    : TEXCOORD2;
    float3 view     : TEXCOORD3;
	float4 shadow   : TEXCOORD4;
	float4 depth 	: TEXCOORD5;
	float3 tangent  : TEXCOORD6;
	float3 binormal : TEXCOORD7;
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
float4x4 make_bias_mat(float BiasVal)
{
	float fTexWidth = screenWidth;
	float fTexHeight = screenHeight;
	// float fZScale = pow(2.0,((float)SHAD_BIT_DEPTH))-1.0; // dx8
	float fZScale = 1.0; //dx9
	float fOffsetX = 0.5f + (0.5f / fTexWidth);
	float fOffsetY = 0.5f + (0.5f / fTexHeight);
	float4x4 result = float4x4(0.5f,     0.0f,     0.0f,      0.0f,
					0.0f,    -0.5f,     0.0f,      0.0f,
					0.0f,     0.0f,     fZScale,   0.0f,
					fOffsetX, fOffsetY, -BiasVal,     1.0f );
	return result;
}

VS_OUTPUT mainVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
	float4 Po = float4(IN.pos.xyz,1.0);
	const float bsa = 8;
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
	OUT.shadow = mul(mul(gmLightViewProj,OUT.vpos),make_bias_mat(0.000027f));
    float3 wpos = mul(gmWorld, OUT.vpos).xyz;
	OUT.binormal = mul(gmWorld,normalize(cross(OUT.tangent,OUT.normal)));
    OUT.normal  = mul(gmWorld, OUT.normal.xyz);
	OUT.tangent = mul(gmWorld, OUT.tangent.xyz);
	OUT.vpos=mul(gmWorldViewProj,OUT.vpos);
    OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = 1;
	
    OUT.light = normalize(wpos-gvDirLight);
    OUT.view = (gvEye-wpos);
	OUT.depth.xyz = -wpos;
	OUT.depth.w = -OUT.vpos.z;
    return OUT;
}

VS_OUTPUT DeferredVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
	float4 Po = float4(IN.pos.xyz,1.0);
	const float bsa = 8;
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
	OUT.shadow = mul(mul(gmLightViewProj,OUT.vpos),make_bias_mat(0.000027f));
    float3 wpos = mul(gmWorld, OUT.vpos).xyz;
	OUT.binormal = mul(gmWorld,normalize(cross(OUT.tangent,OUT.normal)));
    OUT.normal  = mul(gmWorld, OUT.normal.xyz);
	OUT.tangent = mul(gmWorld, OUT.tangent.xyz);
	OUT.vpos=mul(gmWorldViewProj,OUT.vpos);
    OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
    OUT.light = normalize(wpos-gvDirLight);
    OUT.view = (gvEye-wpos);
	OUT.depth.xyz = wpos;
	OUT.depth.w = length(OUT.shadow);
    return OUT;
}

VS_SHADOW_OUTPUT shadowVS(VS_INPUT IN)
{
    VS_SHADOW_OUTPUT OUT;
	float4 Po = float4(IN.pos.xyz,1.0);
	const float bsa = 8;
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
float4 mainPS(VS_OUTPUT IN) : COLOR
{
  float3 vNormal = (tex2D( gsNormals, IN.texcoord.xy ));
  vNormal = 2 * vNormal - 1.0;
  float4 texColor = tex2D(gsDiffuse, IN.texcoord.xy);
  float3x3 mTangentToWorld = transpose( float3x3( IN.tangent, IN.binormal, IN.normal ) );
  float3   vNormalWorld    = normalize( mul( mTangentToWorld, vNormal ));
  float4 color = texColor * gvPaintColor;
  float3 H = normalize(IN.light + normalize(IN.view));
  float n  = 16;
  float D  = dot(vNormalWorld, H);
  float shadow = 0;
	for (int y=-1; y<1; y++){
		for (int x=-1; x<1; x++)
		{
			float4 coord = IN.shadow;
			coord.xy += (float2(x,y)/((screenWidth+screenHeight)/2))*IN.shadow.w;
			shadow +=tex2Dproj( shadowSampler, coord );
		}
	}
	shadow/= 4;
  float specular = D/(n-D*n+D);
  specular *= tex2D( gsSpecular, IN.texcoord.xy ).x;
  float shadowingTerm = saturate((max(dot(IN.light,vNormalWorld),0.0)*0.5)*shadow);
  float3 ambient =saturate(lerp(gvAmbientColor2.xyz,gvAmbientColor.xyz,vNormalWorld.z));
  float3 shadedColor = color * (shadowingTerm+ambient*0.3);
  float4 finalColor;
  finalColor.xyz = shadedColor.xyz + specular * 0.5 * shadow;
  finalColor.a = color.a;
  return finalColor;
}
Deferred_OUT DeferredPS(VS_OUTPUT IN)
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
	float shadow = 0;
	for (int y=-1; y<1; y++){
		for (int x=-1; x<1; x++)
		{
			float4 coord = IN.shadow;
			coord.xy += (float2(x,y)/((screenWidth+screenHeight)/2))*IN.shadow.w;
			shadow +=tex2Dproj( shadowSampler, coord );
		}
	}
	shadow/= 4;
	OUT.col2 = float4(IN.depth.xyz,1);
	//OUT.col3 = float4(1,1,1,1);
	return OUT;
}

technique Forward
{
    pass p0
    {
        VertexShader = compile vs_3_0 mainVS();
        PixelShader  = compile ps_3_0 mainPS();
    }
};
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