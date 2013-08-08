#pragma once
#include "RenderWare.h"
#include "game\CVector.h"
#include "D3D9Headers\d3d9.h"
#include "D3D9Headers\d3dx9.h"

#define MAX_GAME_LIGHTS 50

typedef class CLights CLights;
typedef class CLight CLight;

// Default SA light type for our light
enum eLightMode
{
	LIGHT_MODE_POINT,
	LIGHT_MODE_DIRECTIONAL,
	LIGHT_MODE_SHADOW,
	LIGHT_MODE_GLOBAL = 4
};

// Our type
enum eLightType
{
	LIGHT_TYPE_OMNI
};

class CLights
{
public:
	static unsigned int m_nNumLights;
	static CLight m_aLights[MAX_GAME_LIGHTS];
	static bool AddOmniLight(eLightMode, CVector, CVector, float, float, float, float, unsigned char, bool, void *);
	static void Patch();
};

class CLight
{
public:
	CVector pos;
	CVector dir;
	float radius;
	float red, green, blue;
	eLightMode mode;
	eLightType type;
	unsigned char fogType;
	bool generateShadows;
};