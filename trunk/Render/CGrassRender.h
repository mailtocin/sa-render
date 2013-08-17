#pragma once
#include <d3d9.h>
#include "CGame.h"

class CGrassRender
{
public:
	static ID3DXEffect *m_pEffect;
	static bool Setup();
	static void InitGrassShader();
	static void DeInitGrassShader();
	static void Reset();
	static void Lost();
};

