#pragma once
#include <d3d9.h>
#include "CGame.h"

class CParticleRender
{
public:
	static ID3DXEffect *m_pEffect;
	static bool Setup();
	static void InitParticleShader();
	static void DeInitParticleShader();
	static void Reset();
	static void Lost();
	static DWORD oDB,oSB,oBO,oAB,oAT;
};

