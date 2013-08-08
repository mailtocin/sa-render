#pragma once
#include "RenderWare.h"
#include <d3dx9.h>

class CVehicleRender
{
	static void RenderCB(RwResEntry *repEntry, RpAtomic *atomic, unsigned char type, char flags);
	static void Update();
public:
	static void Patch();
	static bool Setup();
	static void Reset();
	static void Lost();
	static ID3DXEffect *m_pEffect;
	static D3DXMATRIX m_LightViewProj;
	static IDirect3DTexture9 *defnormal;
	static IDirect3DTexture9 *defspec;
};