#include "CWaterRender.h"
#include "CDebug.h"
#include <d3d9.h>
ID3DXEffect *CWaterRender::m_pEffect;
IDirect3DTexture9 *CWaterRender::tNormalW;
IDirect3DTexture9 *CWaterRender::tNormalN;
IDirect3DTexture9 *CWaterRender::tNormalH1;
IDirect3DTexture9 *CWaterRender::FoamTexture;
IDirect3DTexture9 *CWaterRender::FoamTexture2;
DWORD CWaterRender::oDB,CWaterRender::oSB,CWaterRender::oBO,CWaterRender::oAB,CWaterRender::oAT;

bool CWaterRender::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"resources/Shaders/water.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CWaterRender", "water", result))
	{
		return false;
	}
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/Water_NormalN.png",&tNormalN);
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/Water_NormalW.png",&tNormalW);
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/Waves.png",&tNormalH1);
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/Foam.png",&FoamTexture);
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/Foam.dds",&FoamTexture2);
	return true;
}

void CWaterRender::InitWaterShader() {
	D3DXMATRIX vp;
	IDirect3DBaseTexture9 *diffuse;
	UINT p;
	getWorldViewProj(NULL,NULL,&vp);
	D3DXMATRIX mView,mInvView;
	g_Device->GetTransform(D3DTS_VIEW,&mView);
	D3DXMatrixInverse(&mInvView,NULL,&mView);
	g_Device->GetTexture(0,&diffuse);
	int Hours = (int)(*(BYTE *)0xB70153);
	int Mins = (int)(*(BYTE *)0xB70152);
	int Secs = (int)(*(BYTE *)0xB70150);
	float f_Time = ((float)Hours+(float)Mins/60.0f+(float)Secs/60/60)/24.0f;
	m_pEffect->SetMatrix("gmViewProj",&vp);
	m_pEffect->SetMatrix("gmInvView",&mInvView);
	m_pEffect->SetTexture("gtDiffuse",diffuse);
	m_pEffect->SetTexture("tNormalN",tNormalN);
	m_pEffect->SetTexture("tNormalW",tNormalW);
	m_pEffect->SetTexture("tNormalH1",tNormalH1);
	m_pEffect->SetTexture("FoamTexture",FoamTexture);
	m_pEffect->SetTexture("FoamTexture2",FoamTexture2);
	m_pEffect->SetVector("g_LightDir",&CGlobalValues::gm_SunPosition);
	m_pEffect->SetVector("fInverseViewportDimensions",&D3DXVECTOR4(1.0f/RsGlobal->MaximumWidth,1.0f/RsGlobal->MaximumHeight,0.0f,0.0f));
	m_pEffect->SetFloat("time",f_Time);
	m_pEffect->SetTechnique("Forward");
	GetCurrentStates();
	m_pEffect->Begin(&p,0);
	m_pEffect->BeginPass(0);
}
void CWaterRender::DeInitWaterShader() {
	m_pEffect->EndPass();
	m_pEffect->End();
	SetOldStates();
}
void CWaterRender::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}

void CWaterRender::Lost()
{
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}