#pragma once
#include "Headers\rapidXML\rapidxml.hpp"
#include "Headers\rapidXML\rapidxml_utils.hpp"
#include "Headers\CGame.h"
struct TCRegion{
	char* cName;
	D3DXVECTOR3 LightDirCol[12];
	float LightDirMult[12];
	D3DXVECTOR3 NaturAmbColUp[12];
	D3DXVECTOR3 NaturAmbColDown[12];
	float FarClip[12];
	D3DXVECTOR3 SkyZenith[12];
	D3DXVECTOR3 SkyAzimuth[12];
};

class CGTAVTimeCycle
{
public:
	static int m_nCurrentTimeID;
	static int m_nNextTimeID;
	static float A;
	static float B;
	static void ParseTC();
	static void UptadteTime();
	static D3DXVECTOR4 GetCurrentDirectLightColor();
	static D3DXVECTOR4 GetCurrentNatAmbLightColorUp();
	static D3DXVECTOR4 GetCurrentNatAmbLightColorDown();
	static float GetCurrentFarClip();
	static TCRegion currRegion;
};

