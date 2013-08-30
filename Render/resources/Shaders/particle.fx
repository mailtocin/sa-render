float4x4 gmViewProj;
float4x4 gmInvProj;
texture2D gtDiffuse;
texture2D gtDepth;
float2 fInverseViewportDimensions = {1.0/1024.0,1.0/768.0};
float3 g_LightDir ={0,0,1};
float3 g_vRight;
float3 g_vUp;
float3 g_vForward;
float3 gvAmbientColor;
float3 gvAmbientColor2;

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

struct VS_OUTPUT
{
    float4 vpos     : POSITION;
    float3 texcoord : TEXCOORD0;
	float4 color    : TEXCOORD1;
	float3 wpos     : TEXCOORD2;
};

struct VS_INPUT
{
    float4 pos      : POSITION;
    float2 texcoord : TEXCOORD0;
	float4 color    : COLOR;
};

VS_OUTPUT ForwardVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    OUT.vpos=mul(gmViewProj,float4(IN.pos.xyz,1.0));
	OUT.color = IN.color;
    OUT.texcoord.xy = IN.texcoord.xy;
	OUT.texcoord.z = OUT.vpos.z;
	OUT.wpos = IN.pos.xyz;
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

float4 ForwardPS(VS_OUTPUT IN,float2 viewpos:VPOS) : COLOR0
{
	float4 texColor = tex2D(gsDiffuse, IN.texcoord.xy);
	float3 norm = AutoNormalGen(gsDiffuse, IN.texcoord.xy) * 2 - 1;
	norm.xy *= 3;
	float3 worldnorm;
	worldnorm = -norm.x * g_vRight;
    worldnorm += norm.y * g_vUp;
    worldnorm += -norm.z * g_vForward;
	float3 lighting = max(dot(normalize(worldnorm),-normalize(g_LightDir-IN.wpos)),0.0f)+lerp(gvAmbientColor,gvAmbientColor2,saturate(worldnorm.z))*0.5f;
	float depthFade = 1;
	float depth = tex2D(gsDepth,viewpos*fInverseViewportDimensions + fInverseViewportDimensions*0.5f).w;
	float4 depthViewSample = float4( viewpos, depth, 1 );
    float4 depthViewParticle = float4( viewpos, IN.texcoord.z, 1 );
	float depthDiff = depthViewSample.z/depthViewSample.w - depthViewParticle.z/depthViewParticle.w;
	depthFade = saturate( depthDiff / 1.2f );
	texColor.w*=depthFade;
	return float4(texColor.xyz*lighting*IN.color.xyz,texColor.w*IN.color.w);
}

technique Forward
{
    pass p0
    {
        VertexShader = compile vs_3_0 ForwardVS();
        PixelShader  = compile ps_3_0 ForwardPS();
		CULLMODE = none;
		SCISSORTESTENABLE = FALSE;
		AlphaBlendEnable=TRUE;
		ALPHATESTENABLE=FALSE;
		ZEnable=TRUE;
		//ALPHAFUNC = GREATEREQUAL;
		//ALPHAREF = 0x0000000F;
		//SrcBlend = SRCALPHA;
		//DestBlend = INVSRCALPHA;
    }
};