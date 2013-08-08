#pragma once
#include "D3D9Headers\d3d9.h"
#include "D3D9Headers\d3dx9.h"
#include "RenderWare.h"
class CDeferredRendering
{
public:
	static bool Setup();
	static void Reset();
	static void Lost();
	static void Patch();
	// TODO: Move it to CGame module
	static void Idle(void *);
	static void RenderScene();
	static void DrawPostProcessPass();
	static void PostProcess(IDirect3DSurface9 *outSurf);
	static void CreateShadowCamera();
	static void ComputeShadowMap(IDirect3DSurface9*shadowSurface,IDirect3DSurface9*shadowSurfaceC,float distance,D3DXMATRIX*lightview,D3DXMATRIX*lightproj,float dd);
	static void DestroyShadowCamera();
	static void CalculateSplitDistances(float NearPlane,float FarPlane);
	static void DrawCubemap();
	static IDirect3DTexture9 *shadow[4];
	static IDirect3DSurface9 *shadowSurface[4];
	static IDirect3DTexture9 *gbuffer[3];
	static IDirect3DSurface9 *gbSurface[3];
	static IDirect3DTexture9 **rtTmpSurface;
	static IDirect3DSurface9 **rsTmpSurface;
	static IDirect3DTexture9 *lightingTexture;
	static IDirect3DSurface9 *lightingSurface;
	static IDirect3DCubeTexture9 *cubemap;
	static int *surfWidth;
	static int *surfHeight;
	static int ppTCcount;
	static RwCamera *m_pShadowCamera;
	static D3DXMATRIX g_mLightView[2];
	static D3DXMATRIX g_mLightProj;
	static D3DXMATRIX g_mLightProj2;
	static ID3DXEffect *m_pEffect;
	static bool m_baSplitColorChannels[4][4];
};