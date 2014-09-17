#include "..\Headers\CLight.h"
#include "..\Headers\CGame.h"
#include "patch\CPatch.h"

unsigned int CLights::m_nNumLights;
CLight CLights::m_aLights[MAX_GAME_LIGHTS];


signed short currVehicleID = 0;
void CLights::Patch()
{
	CPatch::RedirectCall(0x47B0D8, AddOmniLight);
	CPatch::RedirectCall(0x48ED76, AddOmniLight);
	CPatch::RedirectCall(0x49DF47, AddOmniLight);
	CPatch::RedirectCall(0x53632D, AddOmniLight);
	CPatch::RedirectCall(0x5364B4, AddOmniLight);
	CPatch::RedirectCall(0x53AEAC, AddOmniLight);
	CPatch::RedirectCall(0x6AB80F, AddOmniLight);
	CPatch::RedirectCall(0x6ABBA6, AddOmniLight);
	CPatch::RedirectCall(0x6BD641, AddOmniLight);
	CPatch::RedirectCall(0x6D4D14, AddOmniLight);

	CPatch::RedirectCall(0x6E27E6, AddOmniSpotLight);
	CPatch::RedirectCall(0x6E28E7, AddOmniLight);
	CPatch::RedirectCall(0x6E1A76, GetVehID);

	

	CPatch::RedirectCall(0x6FD105, AddOmniLight);
	CPatch::RedirectCall(0x6FD347, AddOmniLight);
	CPatch::RedirectCall(0x737849, AddOmniLight);
	CPatch::RedirectCall(0x7378C1, AddOmniLight);
	CPatch::RedirectCall(0x73AF74, AddOmniLight);
	CPatch::RedirectCall(0x73CCFD, AddOmniLight);
	CPatch::RedirectCall(0x740D68, AddOmniLight);
}

bool CLights::AddOmniLight(eLightMode defaultType, CVector pos, CVector dir, float radius, float red, float green, float blue, unsigned char fogType,
	bool generateExtraShadows, void *entityAffected)
{
	
	if (m_nNumLights < MAX_GAME_LIGHTS)
	{
		m_aLights[m_nNumLights].pos.x = pos.x;
		m_aLights[m_nNumLights].pos.y = pos.y;
		m_aLights[m_nNumLights].pos.z = pos.z;
		m_aLights[m_nNumLights].dir.x = max(min(dir.x, 1), -1) * 2;
		m_aLights[m_nNumLights].dir.y = max(min(dir.y, 1), -1) * 2;
		m_aLights[m_nNumLights].dir.z = max(min(dir.z, 1), -1) * 2;
		m_aLights[m_nNumLights].radius = radius;
		m_aLights[m_nNumLights].red = red;
		m_aLights[m_nNumLights].green = green;
		m_aLights[m_nNumLights].blue = blue;
		m_aLights[m_nNumLights].mode = defaultType;
		m_aLights[m_nNumLights].generateShadows = generateExtraShadows;
		m_aLights[m_nNumLights].fogType = fogType;
		if (defaultType != LIGHT_MODE_DIRECTIONAL)
			m_aLights[m_nNumLights].type = LIGHT_TYPE_OMNI;
		else
			m_aLights[m_nNumLights].type = LIGHT_TYPE_SPOT;
		m_nNumLights++;
		return true;
	}
	else
		return false;
}
bool CLights::AddOmniSpotLight(eLightMode defaultType, CVector pos, CVector dir, float radius, float red, float green, float blue, unsigned char fogType,
	bool generateExtraShadows, void *entityAffected)
{
	// Read pointer to modelPtrs
	

	CVehicleModelInfo* pModelinfo = (CVehicleModelInfo*)GetModelPtrs()[currVehicleID];

	if (m_nNumLights < MAX_GAME_LIGHTS)
	{
		RwV3d headlightPos = pModelinfo->m_pVehicleStruct->m_vDummyPos[2];
		m_aLights[m_nNumLights].pos.x = pos.x + max(min(dir.x, 1), -1) * 1.8f;
		m_aLights[m_nNumLights].pos.y = pos.y + max(min(dir.y, 1), -1) * 1.8f;
		m_aLights[m_nNumLights].pos.z = pos.z + max(min(dir.z, 1), -1) * 1.8f;
		m_aLights[m_nNumLights].dir.x = max(min(dir.x, 1), -1) * 2;
		m_aLights[m_nNumLights].dir.y = max(min(dir.y, 1), -1) * 2;
		m_aLights[m_nNumLights].dir.z = max(min(dir.z, 1), -1) * 2;
		m_aLights[m_nNumLights].radius = radius;
		m_aLights[m_nNumLights].red = red;
		m_aLights[m_nNumLights].green = green;
		m_aLights[m_nNumLights].blue = blue;
		m_aLights[m_nNumLights].mode = defaultType;
		m_aLights[m_nNumLights].generateShadows = generateExtraShadows;
		m_aLights[m_nNumLights].fogType = fogType;
		m_aLights[m_nNumLights].type = LIGHT_TYPE_SPOT;
		m_nNumLights++;
		return true;
	}
	else
		return false;
}
char __fastcall CLights::GetVehID(void *pVehicle){
	currVehicleID = ((CEntity*)pVehicle)->m_wModelIndex;
	return GetVehicleLightsStatus(pVehicle);
}