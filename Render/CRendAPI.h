#include "CDeferredRendering.h"
#include <stdio.h>
#include <sstream>
#ifdef RENDER_EXPORTS
#define RENDER_API __declspec(dllexport) 
#else
#define RENDER_API __declspec(dllimport) 
#endif
RENDER_API int __cdecl setShadowDistance(float distance)
{
	CDeferredRendering::maxShadowDistance = distance;
	return TRUE;
}

RENDER_API float __cdecl getShadowDistance()
{
	return CDeferredRendering::maxShadowDistance;
}

RENDER_API int __cdecl getShadowMapSize()
{
	return CDeferredRendering::ShadowMapSize;
}

RENDER_API int __cdecl setShadowMapSize(int size)
{
	CDeferredRendering::ShadowMapSize = size;
	return TRUE;
}

RENDER_API float __cdecl getShadowBias()
{
	return CDeferredRendering::ShadowBias;
}

RENDER_API int __cdecl setShadowBias(float bias)
{
	CDeferredRendering::ShadowBias = bias;
	return TRUE;
}

RENDER_API int __cdecl loadINI()
{
	char cStr[256];
	char cPath[MAX_PATH];
	GetModuleFileName(NULL, cPath, MAX_PATH);
	if (strrchr(cPath, '\\')) *(char*)(strrchr(cPath, '\\') + 1) = '\0';
	strcat_s(cPath, MAX_PATH, "resources/SARender.ini");
	GetPrivateProfileString("SHADOWS", "MaxShadowDistance", "1500", cStr, 256, cPath);
	CDeferredRendering::maxShadowDistance = (float)atof(cStr);
	GetPrivateProfileString("SHADOWS", "ShadowMapSize", "4096", cStr, 256, cPath);
	CDeferredRendering::ShadowMapSize = (int)atoi(cStr);
	GetPrivateProfileString("SHADOWS", "ShadowBias", "0.0005", cStr, 256, cPath);
	CDeferredRendering::ShadowBias = (float)atof(cStr);
	return TRUE;
}

RENDER_API int __cdecl saveINI()
{
	char cPath[MAX_PATH];
	GetModuleFileName(NULL, cPath, MAX_PATH);
	if (strrchr(cPath, '\\')) *(char*)(strrchr(cPath, '\\') + 1) = '\0';
	strcat_s(cPath, MAX_PATH, "resources/SARender.ini");
	WritePrivateProfileString("SHADOWS", "MaxShadowDistance",std::to_string((long double)CDeferredRendering::maxShadowDistance).c_str(), cPath);
	WritePrivateProfileString("SHADOWS", "ShadowMapSize", std::to_string((long double)CDeferredRendering::ShadowMapSize).c_str(), cPath);
	WritePrivateProfileString("SHADOWS", "ShadowBias", std::to_string((long double)CDeferredRendering::ShadowBias).c_str(), cPath);
	return TRUE;
}