float4x4 gmViewProj;
float4x4 gmInvView;
texture2D gtDiffuse;
texture2D gtDepth;
float2 fInverseViewportDimensions = {1.0/1024.0,1.0/768.0};
float3 g_LightDir ={0,0,1};
float3 g_vRight;
float3 g_vUp;
float3 g_vForward;
float3 gvAmbientColor;
float3 gvAmbientColor2;
float Softness 	= 0.2f;
float2 WaterScale	= { 100, 100 };
float4 WaterSpeed = { -0.004f, -0.0045f, 0.1f, 0.02f };
float3 RefractionColor = {1,1,1};
float3x3 TBN = {float3(1,0,0),float3(0,1,0),float3(0,0,1)};
float2 TexScale = { 1.0f, 1.0f };
float  time;

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
const texture tScreen;
sampler TexScreen=sampler_state {
    Texture   = <tScreen>;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
    ADDRESSW  = CLAMP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};

// Normal Textures /////////////////////////////////////////
const texture tNormalW;
sampler TexNormalW=sampler_state {
    Texture   = <tNormalW>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};
const texture tNormalN;
sampler TexNormalN=sampler_state {
    Texture   = <tNormalN>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};

const texture tNormalH1;
sampler TexWaves=sampler_state {
    Texture   = <tNormalH1>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = LINEAR;
};

const texture FoamTexture;
sampler FoamT=sampler_state {
    Texture   = <FoamTexture>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = POINT;
};

const texture FoamTexture2;
sampler FoamT2=sampler_state {
    Texture   = <FoamTexture2>;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
    ADDRESSW  = WRAP;
    MAGFILTER = LINEAR;
    MINFILTER = LINEAR;
    MIPFILTER = POINT;
};

struct VS_OUTPUT
{
    float4 vpos     : POSITION;
    float3 texcoord : TEXCOORD0;
	float3 wpos     : TEXCOORD1;
	float4 Tex 		: TEXCOORD2;
	float2 Tex2 	: TEXCOORD3;
 	float2 Tex3   	: TEXCOORD4;
	float3 FresVec	: TEXCOORD5;
};

struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 texcoord : TEXCOORD0;
};

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
	OUT.Tex2					= WaterUv2;	
	OUT.Tex3					= WaterUv3;	
	float3 ViewPos = gmInvView[3].xyz - OUT.wpos;	
	OUT.FresVec = mul( ViewPos, TBN );
	OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
    return OUT;
}

float3 AutoNormalGen(sampler2D sample,float2 texCoord) {
   float off = 1.0 / 8;
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

float PhongSpecular(float3 normal, float3 viewDir, float specularDecay,float3 LightDir)
{
    float nDotL = dot(normal, LightDir);
    float3 reflection = 2.0f * normal * nDotL + -LightDir;
    reflection = normalize(reflection);
    float rdotV = saturate(dot(reflection, viewDir));
    return pow(rdotV, specularDecay);
}

float4 ForwardPS(VS_OUTPUT IN,float2 viewpos:VPOS) : COLOR0
{
	float4 texColor = tex2D(gsDiffuse, IN.texcoord.xy);
	
	float3 norm = AutoNormalGen(gsDiffuse, IN.texcoord.xy);
	float3 NormalWater;
	NormalWater				= tex2D( TexWaves, IN.Tex2 *.5 ) * 0.30f;
	NormalWater				+= tex2D( TexWaves, IN.Tex3 ) * 0.30f;	

	NormalWater			 	+= tex2D( TexNormalW, IN.Tex / ( TexScale ) + ( time * WaterSpeed.zw ) ) * 0.20f;
	NormalWater			 	+= tex2D( TexNormalN, IN.Tex / ( TexScale ) - ( time * WaterSpeed.zw ) ) * 0.20f;

	NormalWater				= normalize( NormalWater * 2.0f - 1.0f );
	
	float depthFade = 1;
	float Dist = distance(IN.wpos,gmInvView[3].xyz);
	float depth = tex2D(gsDepth,viewpos*fInverseViewportDimensions + fInverseViewportDimensions*0.5f).w;
	float4 depthViewSample = float4( viewpos, depth, 1 );
    float4 depthViewParticle = float4( viewpos, IN.texcoord.z, 1 );
	float depthDiff = depthViewSample.z/depthViewSample.w - depthViewParticle.z/depthViewParticle.w;
	depthDiff *= Dist * ( Softness * Dist );
	depthDiff = saturate( depthDiff );
	float3 PosCamera		= normalize( gmInvView[3].xyz - IN.wpos );
	float3 normalWorld 	= normalize((float3(0, 0, 1) * NormalWater.z) + (NormalWater.x * float3(0, 1, 0) + NormalWater.y * float3(-1, 0, 0)));
	float Specular 		= PhongSpecular( normalWorld, -PosCamera, 256 , g_LightDir-IN.wpos);
	float2 refrTC = (NormalWater.xy *(0.1+ 0.1));
	float4 Refra	= 0;
	Refra.x 			= tex2D(TexScreen,  (viewpos*fInverseViewportDimensions + fInverseViewportDimensions*0.5f) + refrTC * 1.15 ).x;
	Refra.y 			= tex2D(TexScreen,  (viewpos*fInverseViewportDimensions + fInverseViewportDimensions*0.5f) + refrTC ).y;
	Refra.z 			= tex2D(TexScreen,  (viewpos*fInverseViewportDimensions + fInverseViewportDimensions*0.5f) + refrTC * 0.85 ).z;
	Refra 				= (( 1.0 - depthDiff ) + float4( RefractionColor, 1 ) ) * Refra;
	float3 Fresnel 	= dot( NormalWater, normalize( IN.FresVec ) );
	Fresnel 				= lerp( 0, float3(1,.8,.8), Fresnel );
	float3 Water 	= lerp( texColor.xyz, Refra, Fresnel );
	//texColor.w*=(depthDiff*7);
	return float4(Water+Specular,depthDiff*7);
}

technique Forward
{
    pass p0
    {
        VertexShader = compile vs_3_0 ForwardVS();
        PixelShader  = compile ps_3_0 ForwardPS();
		CULLMODE = none;
		SCISSORTESTENABLE = FALSE;
		AlphaBlendEnable	= true;
		SrcBlend					= SrcAlpha;
		DestBlend					= InvSrcAlpha;
    }
};