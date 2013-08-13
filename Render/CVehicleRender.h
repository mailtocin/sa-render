#pragma once
#include "RenderWare.h"
#include "D3D9Headers\d3dx9.h"
#include "CGame.h"

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
};