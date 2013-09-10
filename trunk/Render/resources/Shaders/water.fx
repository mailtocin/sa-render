#include "Helpers.fx"
//--------------------------------Matrix's--------------------------------
float4x4 gmViewProj;
float4x4 gmInvView;
float4x4 gmRefl;
float3x3 TBN = {float3(1,0,0),float3(0,1,0),float3(0,0,1)};
//--------------------------------Textures--------------------------------
const texture gtDiffuse;
const texture gtDepth;
const textureCUBE tRefl;
const texture tScreen;
const texture tNormalW;
const texture tNormalN;
const texture tNormalH1;
const texture FoamTexture;
const texture FoamTexture2;
//--------------------------------Vectors---------------------------------
float3 g_LightDir ={0,0,1};
float3 g_vRight;
float3 g_vUp;
float3 g_vForward;
float3 gvAmbientColor;
float3 gvAmbientColor2;
float2 WaterScale	= { 100, 100 };
float4 WaterSpeed = { -0.004f, -0.0045f, 0.1f, 0.02f };
float3 RefractionColor = {1,1,1};
float2 TexScale = { 1.0f, 1.0f };
//--------------------------------Scalars---------------------------------
float  time;
float Softness 	= 0.2f;
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
sampler TexScreen=sampler_state {
    Texture   = <tScreen>;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
    ADDRESSW  = CLAMP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
samplerCUBE TexRefl=sampler_state {
    Texture   = <tRefl>;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
    ADDRESSW  = CLAMP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler TexNormalW=sampler_state {
    Texture   = <tNormalW>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler TexNormalN=sampler_state {
    Texture   = <tNormalN>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler TexWaves=sampler_state {
    Texture   = <tNormalH1>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler FoamT=sampler_state {
    Texture   = <FoamTexture>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
sampler FoamT2=sampler_state {
    Texture   = <FoamTexture2>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};

struct VS_OUTPUT
{
    float4 vpos     : POSITION;
    float3 texcoord : TEXCOORD0;
	float3 wpos     : TEXCOORD1;
	float4 Tex 		: TEXCOORD2;
	float4 Tex2 	: TEXCOORD3;
 	float2 Tex3   	: TEXCOORD4;
	float3 FresVec	: TEXCOORD5;
	float4 refl 	: TEXCOORD6;
};

struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 texcoord : TEXCOORD0;
};

float4x4 make_bias_mat(float BiasVal)
{
	float fTexWidth = 1024;
	float fTexHeight = 768;
	// float fZScale = pow(2.0,((float)SHAD_BIT_DEPTH))-1.0; // dx8
	float fZScale = 1.0; //dx9
	float fOffsetX = 0.5f + (0.5f / fTexWidth);
	float fOffsetY = 0.5f + (0.5f / fTexHeight);
	float4x4 result = float4x4(0.5f,     0.0f,     0.0f,      0.0f,
							   0.0f,    -0.5f,     0.0f,      0.0f,
							   0.0f,     0.0f,     fZScale,   0.0f,
							   fOffsetX, fOffsetY, BiasVal,     1.0f );
	return result;
}

VS_OUTPUT ForwardVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.vpos=mul(gmViewProj,float4(IN.pos.xyz,1.0));
	OUT.wpos = IN.pos.xyz;
	float2 Uv = ( OUT.wpos.xy / WaterScale );
	float4 WaterUv;
				 WaterUv.xy = Uv + ( time * WaterSpeed.xy );
				 WaterUv.zw = Uv + ( time * WaterSpeed.zw );

	float2 WaterUv2;
				 WaterUv2.xy = Uv - ( time * ( WaterSpeed.xy * 10 ) );	       
	float2 WaterUv3;
				 WaterUv3.xy = Uv + ( time * ( WaterSpeed.xy * 10 ) );
	OUT.Tex 					= WaterUv;	
	OUT.Tex2.xy					= WaterUv2;	
	OUT.Tex3					= WaterUv3;	
	float3 ViewPos = gmInvView[3].xyz - OUT.wpos;
	OUT.Tex2.zw = CalculateVPos(OUT.vpos);
	OUT.FresVec = mul( ViewPos, TBN );
	OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
	float4x4 bias_mat = make_bias_mat(0.0000f);
	OUT.refl = float4( OUT.vpos.x * 0.5 + 0.5 * OUT.vpos.w, 0.5 * OUT.vpos.w + OUT.vpos.y * 0.5, OUT.vpos.w, OUT.vpos.w );
    return OUT;
}

float PhongSpecular(float3 normal, float3 viewDir, float specularDecay,float3 LightDir)
{
    float nDotL = dot(normal, LightDir);
    float3 reflection = 2.0f * normal * nDotL + -LightDir;
    reflection = normalize(reflection);
    float rdotV = saturate(dot(reflection, viewDir));
    return pow(rdotV, specularDecay);
}

float4 ForwardPS(VS_OUTPUT IN) : COLOR0
{
	float3 NormalWater;
	NormalWater				= tex2D( TexWaves, IN.Tex2.xy *.5 ) * 0.30f;
	NormalWater				+= tex2D( TexWaves, IN.Tex3 ) * 0.30f;	

	NormalWater			 	+= tex2D( TexNormalW, IN.Tex / ( TexScale ) + ( time * WaterSpeed.zw ) ) * 0.20f;
	NormalWater			 	+= tex2D( TexNormalN, IN.Tex / ( TexScale ) - ( time * WaterSpeed.zw ) ) * 0.20f;

	NormalWater				= normalize( NormalWater * 2.0f - 1.0f );
	float3 normalWorld 	= normalize((float3(0, 0, 1) * NormalWater.z) + (NormalWater.x * float3(1, 0, 0) + NormalWater.y * float3(0, 1, 0)));
	normalWorld.xy *= 0.3f;
	float3 PosCamera		= normalize( gmInvView[3].xyz - IN.wpos );
	float4 texColor = texCUBE(TexRefl, -reflect(PosCamera,normalWorld.xyz));
	float Specular 		= PhongSpecular( normalWorld, -PosCamera, 256 , g_LightDir-IN.wpos);
	float2 refrTC = (NormalWater.xy *(0.1+ 0.1));
	float4 Refra	= 0;
	Refra.x 			= tex2D( TexScreen, IN.Tex2.zw + refrTC * 1.15 ).x;
	Refra.y 			= tex2D( TexScreen, IN.Tex2.zw + refrTC ).y;
	Refra.z 			= tex2D( TexScreen, IN.Tex2.zw + refrTC * 0.85 ).z;
	float3 Fresnel 	= dot( NormalWater, normalize( IN.FresVec ) );
	Fresnel 				= lerp( 0, float3(1,.8,.8), Fresnel );
	float3 Water 	= lerp( texColor.xyz, Refra, Fresnel );
	//texColor.w*=(depthDiff*7);
	return float4(Water+Specular,1.0f);
}

technique Forward
{
    pass p0
    {
        VertexShader = compile vs_2_0 ForwardVS();
        PixelShader  = compile ps_2_0 ForwardPS();
		AlphaTestEnable = false;
		AlphaBlendEnable = true;
    }
};