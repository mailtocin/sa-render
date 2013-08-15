#pragma once
#include <d3d9.h>
#include "CGame.h"

class CGrassRender
{
public:
	static HRESULT DrawGrass(int, int, IDirect3DDevice9 *, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
};

