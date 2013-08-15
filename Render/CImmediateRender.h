#pragma once
#include <d3d9.h>
#include "CGame.h"

enum eImmediateRendering
{
	IM_RENDER_NOT_DEFINED = 0,
	IM_RENDER_WATER = 1,
	IM_RENDER_GRASS = 2,
};

class CImmediateRender
{
public:
	static eImmediateRendering m_nCurrentRendering;
	static void Patch();
	static HRESULT __fastcall DrawPrimitive_1(int, int, IDirect3DDevice9 *, D3DPRIMITIVETYPE, UINT, UINT);
	static HRESULT __fastcall DrawPrimitive_2(int, int, IDirect3DDevice9 *, D3DPRIMITIVETYPE, UINT, UINT);
	static HRESULT __fastcall DrawPrimitiveUp(int, int, IDirect3DDevice9 *, D3DPRIMITIVETYPE, UINT, const void *, UINT);
	static HRESULT __fastcall DrawIndexedPrimitive(int, int, IDirect3DDevice9 *, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
	static HRESULT __fastcall DrawIndexedPrimitiveUp(int, int, IDirect3DDevice9 *, D3DPRIMITIVETYPE, UINT, UINT, UINT, const void *, D3DFORMAT,
		const void *, UINT);
};