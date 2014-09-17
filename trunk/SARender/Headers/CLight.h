#pragma once
#include "game_sa\RenderWare.h"
#include "game_sa\CVector.h"
#include "game_sa\CMatrix.h"
#include "game_sa\CEntity.h"
#include "game_sa\CBaseModelInfo.h"
#include "game_sa\CVehicleModelInfo.h"
#include <d3d9.h>
#include <d3dx9.h>

#define MAX_GAME_LIGHTS 1000
#define GetVehicleLightsStatus(pVehicle) ((char (__thiscall *)(void *))0x6D55C0)(pVehicle)

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
	LIGHT_TYPE_OMNI,
	LIGHT_TYPE_SPOT
};

class CLights
{
public:
	static unsigned int m_nNumLights;
	static CLight m_aLights[MAX_GAME_LIGHTS];
	static bool AddOmniLight(eLightMode, CVector, CVector, float, float, float, float, unsigned char, bool, void *);
	static bool AddOmniSpotLight(eLightMode, CVector, CVector, float, float, float, float, unsigned char, bool, void *);
	static char __fastcall GetVehID(void *pVehicle);
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