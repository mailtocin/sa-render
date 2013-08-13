#pragma once
#include "D3D9Headers\d3dx9.h"
#include "CTextureMaps.h"

class CRender
{
	static void BeforeReset();
	static void AfterReset();
	static void DefineReset();
	static void Setup();
	static D3DXMATRIX *__cdecl _getComposedMatrix(D3DXMATRIX *m_out);
public:
	static void Patch();
	static void SetTextureMaps(STexture* tex, ID3DXEffect* effect);
	static D3DXMATRIX  m_mTransform;
	static IDirect3DTexture9 *defnormal;
	static IDirect3DTexture9 *defspec;
};