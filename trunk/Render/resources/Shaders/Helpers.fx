#define PI  3.14159265
#define V4ZERO float4(0,0,0,0)
#define V4ONE float4(1,1,1,1)
//--------------------------------Vectors---------------------------------
float2 fInverseViewportDimensions = {1.0/800.0,1.0/800.0};
//--------------------------------IN/OUT structures-----------------------
struct VS_DEFERRED_OUTPUT
{
    float4 vpos     : POSITION;
    float3 texcoord : TEXCOORD0;
    float3 normal   : TEXCOORD1;
	float4 color    : TEXCOORD2;
	float3 tangent  : TEXCOORD3;
	float3 binormal : TEXCOORD4;
	float3 pos 		: TEXCOORD5;
	float2 tex2		: TEXCOORD6;
};
struct Deferred_OUT
{
    float4 albedo    : COLOR0;
    float4 normal 	 : COLOR1;
	float4 position	 : COLOR2;
};
//--------------------------------Helpers---------------------------------
float linerarizeDepth(float Depth, float DOF_NearFocus, float DOF_FarFocus) {
	float CloseDepth=saturate(1-(Depth/DOF_NearFocus));
	return CloseDepth+saturate((-DOF_NearFocus+Depth)/DOF_FarFocus);
}
float2 CalculateVPos ( float4 p ) {
	return float2( (0.5*( float2(p.x + p.w, p.w - p.y) + p.w*fInverseViewportDimensions.xy))/p.w);
}
float2 GetUnsizedVPos ( float2 VPos ) {
	return float2( ( VPos - ( fInverseViewportDimensions * 0.5f ) ) / fInverseViewportDimensions );
}
float2 CalculateNoiseCoords ( float2 tc ) {
	return float2( 1024.0f * tc / 64.0f);
}
float3 GenerateWNormals ( VS_DEFERRED_OUTPUT IN, const sampler2D gsNormals ) {
	float3 vNormal = tex2D( gsNormals, IN.texcoord ).xyz;
	vNormal.z = 1;
	vNormal = 2 * vNormal - 1.0;
	float3x3 mTangentToWorld = transpose( float3x3( IN.tangent, IN.binormal, IN.normal ) );
	return normalize( mul( mTangentToWorld, vNormal ));
}
inline void ComputeAlpha ( in VS_DEFERRED_OUTPUT IN, inout Deferred_OUT OUT, bool bUseVCol, in float clipAlpha ) {
	OUT.albedo.w *= bUseVCol ? IN.color.w : 1.0f;
	OUT.albedo.w = ( OUT.albedo.w < 0.99f && OUT.albedo.w > 0.01f ) ? clipAlpha * OUT.albedo.w : OUT.albedo.w;
	clip(OUT.albedo.w);
}
inline void OutputDeferred ( const sampler2D gsDiffuse, const sampler2D gsNormals, const sampler2D gsSpecular,
							 const sampler2D gsNoise, float gfSpecularFactor, bool bUseVCol, float4 gvColor,
							 inout VS_DEFERRED_OUTPUT IN, inout Deferred_OUT OUT ) {
	float spec,clipAlpha;
	spec = tex2D( gsSpecular, IN.texcoord.xy ).x;
	spec = ( spec > 0 ) ? (spec * gfSpecularFactor) : gfSpecularFactor;
	clipAlpha = 1 - tex2D( gsNoise, CalculateNoiseCoords( IN.tex2 ) ).x;
	OUT.albedo = tex2D(gsDiffuse, IN.texcoord.xy) * gvColor;
	OUT.normal = float4( GenerateWNormals( IN, gsNormals ), spec );
	OUT.position = float4(IN.pos.x,IN.pos.y,IN.pos.z,IN.texcoord.z);
	ComputeAlpha( IN, OUT, bUseVCol, clipAlpha );
}
float3 AutoNormalGen(sampler2D sample, float2 texCoord, float tSize) {
   float off = 1.0 / tSize;
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

   float4 sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
   float4 sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;
   
   float sx = dot(sobelX, lightness);
   float sy = dot(sobelY, lightness);
   
   float3 normal = normalize(float3(sx, sy, 1));
   return normal * 0.5 + 0.5;
}