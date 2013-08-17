#include "CGrassRender.h"
#include "CDebug.h"
#include <d3d9.h>
ID3DXEffect *CGrassRender::m_pEffect;

bool CGrassRender::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"grass.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CGrassRender", "grass", result))
	{
		return false;
	}
	return true;
}

void CGrassRender::InitGrassShader() {
	D3DXMATRIX vp;
	IDirect3DBaseTexture9 *diffuse;
	UINT p;
	getWorldViewProj(NULL,NULL,&vp);
	g_Device->GetTexture(0,&diffuse);
	m_pEffect->SetMatrix("gmViewProj",&vp);
	m_pEffect->SetTexture("gtDiffuse",diffuse);
	m_pEffect->SetTechnique("Forward");
	m_pEffect->Begin(&p,0);
	m_pEffect->BeginPass(0);
}
void CGrassRender::DeInitGrassShader() {
	m_pEffect->EndPass();
	m_pEffect->End();
}
void CGrassRender::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}

void CGrassRender::Lost()
{
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}