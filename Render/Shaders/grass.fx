float4x4 gmViewProj;
texture2D gtDiffuse;
texture2D gtDepth;
float2 fInverseViewportDimensions = {1.0/1024.0,1.0/768.0};
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
    return OUT;
}

float4 ForwardPS(VS_OUTPUT IN,float2 viewpos:VPOS) : COLOR0
{
	float f=1000.0;
	float n = 0.1;
	float depth = tex2D(gsDepth,viewpos*fInverseViewportDimensions + fInverseViewportDimensions*0.5f).w;
	float DepthDiff= depth-IN.texcoord;
	//DepthDiff= min ( 0.5,DepthDiff*0.8);
	DepthDiff = saturate(DepthDiff);
	//DepthDiff = (2 * n) / (f + n - DepthDiff * (f - n));
	float4 texColor = tex2D(gsDiffuse, IN.texcoord.xy)*IN.color;
	//texColor = float4(1,1,1,1);
	texColor.w*=DepthDiff;
	return texColor;
}

technique Forward
{
    pass p0
    {
        VertexShader = compile vs_3_0 ForwardVS();
        PixelShader  = compile ps_3_0 ForwardPS();
		CULLMODE = none;
		SCISSORTESTENABLE = FALSE;
		//AlphaBlendEnable=TRUE;
		//ALPHATESTENABLE=TRUE;
		//ALPHAFUNC = GREATEREQUAL;
		//ALPHAREF = 0x0000000F;
		//SrcBlend = SRCALPHA;
		//DestBlend = INVSRCALPHA;
    }
};