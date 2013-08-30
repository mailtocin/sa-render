#pragma once
#include "CGame.h"
class CPostProcess
{
public:
	static void Patch();
	static bool Setup();
	static void Reset();
	static void Lost();
	static ID3DXEffect *m_pEffect;
	static IDirect3DTexture9 *lensTex;
};

