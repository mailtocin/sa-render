#pragma once
#include "D3D9Headers\d3d9.h"
#include "D3D9Headers\d3dx9.h"
#include "RenderWare.h"
#include "CGame.h"
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
	static void RenderEffects();
	static void RenderParticlesType0();
	// /TODO
	static void DrawPostProcessPass();
	static void PostProcess(IDirect3DSurface9 *outSurf);
	static void CreateShadowCamera();
	static void ComputeShadowMap(IDirect3DSurface9*shadowSurface,IDirect3DSurface9*shadowSurfaceC,float distance,D3DXMATRIX*lightview,D3DXMATRIX*lightproj,int cascadeNum);
	static void DestroyShadowCamera();
	static void DrawCubemap();
	static IDirect3DTexture9 *shadow[8];
	static IDirect3DSurface9 *shadowSurface[8];
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
	static D3DXMATRIX g_mLightView[4];
	static D3DXMATRIX g_mLightProj[4];
	static ID3DXEffect *m_pEffect;
	static bool m_baSplitColorChannels[4][4];
	static float maxShadowDistance;
};