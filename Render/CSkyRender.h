#pragma once
#include "D3D9Headers\d3d9.h"
#include "D3D9Headers\d3dx9.h"
class CSkyRender
{
public:
	static bool Setup();
	static void PreRender(D3DXVECTOR4 *pos,D3DXMATRIX *viewproj);
	static void Render(D3DXVECTOR4 *lightDirection);
	static void Release();
	static bool CreateSkySphere(float fRad,UINT slices,UINT stacks);
	static ID3DXEffect *m_pEffect;
	static IDirect3DTexture9 *CloudTex;
	static IDirect3DTexture9 *StarsTex;
	static IDirect3DTexture9 *StarsMaskTex;
	static D3DXMATRIX gm_WorldViewProjection;
	static D3DXMATRIX gm_World;
	static LPD3DXMESH skySphere;
};

