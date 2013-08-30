#include "CPostProcess.h"
#include "CDebug.h"

IDirect3DTexture9 *CPostProcess::lensTex;
ID3DXEffect *CPostProcess::m_pEffect;
bool CPostProcess::Setup()
{
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/lens.tga",&lensTex);
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"resources/Shaders/skin.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CPedsRender", "skin", result))
	{
		return false;
	}
	return true;
}