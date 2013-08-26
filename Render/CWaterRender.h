#pragma once
#include <d3d9.h>
#include "CGame.h"

class CWaterRender
{
public:
	static ID3DXEffect *m_pEffect;
	static IDirect3DTexture9 *tNormalW;
	static IDirect3DTexture9 *tNormalN;
	static IDirect3DTexture9 *tNormalH1;
	static IDirect3DTexture9 *FoamTexture;
	static IDirect3DTexture9 *FoamTexture2;
	static bool Setup();
	static void InitWaterShader();
	static void DeInitWaterShader();
	static void Reset();
	static void Lost();
	static DWORD oDB,oSB,oBO,oAB,oAT;
};