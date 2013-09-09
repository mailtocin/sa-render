#pragma once
#include "CGame.h"
class CPostProcess
{
public:
	static void Patch();
	static bool Setup();
	static void Reset();
	static void Lost();
	static void Render();
	static void DoBlur();
	static void CreateRTs();
	static ID3DXEffect *m_pEffect;
	static IDirect3DTexture9 *lensTex;
	static IDirect3DTexture9 *screenTex;
	static IDirect3DSurface9 *screenSurf;
	static IDirect3DTexture9 *bluredTex;
	static IDirect3DSurface9 *bluredSurf;
};

