#include "CParticleRender.h"
#include "CDebug.h"
#include <d3d9.h>
ID3DXEffect *CParticleRender::m_pEffect;
DWORD CParticleRender::oDB,CParticleRender::oSB,CParticleRender::oBO,CParticleRender::oAB,CParticleRender::oAT;
bool CParticleRender::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"resources/Shaders/particle.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CParticleRender", "particle", result))
	{
		return false;
	}
	return true;
}

void CParticleRender::InitParticleShader() {
	D3DXMATRIX vp;
	IDirect3DBaseTexture9 *diffuse;
	UINT p;
	getWorldViewProj(NULL,NULL,&vp);
	D3DXMATRIX mView;
	g_Device->GetTransform(D3DTS_VIEW,&mView);
    D3DXVECTOR3 vRight( mView._11, mView._21, mView._31 );
    D3DXVECTOR3 vUp( mView._12, mView._22, mView._32 );
    D3DXVECTOR3 vFoward( mView._13, mView._23, mView._33 );
	g_Device->GetTexture(0,&diffuse);
	m_pEffect->SetMatrix("gmViewProj",&vp);
	m_pEffect->SetTexture("gtDiffuse",diffuse);
	m_pEffect->SetVector("g_vRight",&D3DXVECTOR4(vRight,1.0f));
	m_pEffect->SetVector("g_vUp",&D3DXVECTOR4(vUp,1.0f));
	m_pEffect->SetVector("g_vForward",&D3DXVECTOR4(vFoward,1.0f));
	m_pEffect->SetVector("g_LightDir",&CGlobalValues::gm_SunPosition);
	m_pEffect->SetVector("fInverseViewportDimensions",&D3DXVECTOR4(1.0f/RsGlobal->MaximumWidth,1.0f/RsGlobal->MaximumHeight,0.0f,0.0f));
	D3DXCOLOR ambientColor,ambientColor2;
	ambientColor.r = (float)Timecycle->m_fCurrentAmbientRed;
	ambientColor.g = (float)Timecycle->m_fCurrentAmbientGreen;
	ambientColor.b = (float)Timecycle->m_fCurrentAmbientBlue;
	ambientColor.a = 1.0;
	m_pEffect->SetVector("gvAmbientColor", (D3DXVECTOR4 *)&ambientColor);
	ambientColor2.r = (float)Timecycle->m_fCurrentAmbientObjRed;
	ambientColor2.g = (float)Timecycle->m_fCurrentAmbientObjGreen;
	ambientColor2.b = (float)Timecycle->m_fCurrentAmbientObjBlue;
	ambientColor2.a = 1.0;
	m_pEffect->SetVector("gvAmbientColor2", (D3DXVECTOR4 *)&ambientColor2);
	m_pEffect->SetTechnique("Forward");
	GetCurrentStates();
	m_pEffect->Begin(&p,0);
	m_pEffect->BeginPass(0);
	m_pEffect->CommitChanges();
}
void CParticleRender::DeInitParticleShader() {
	m_pEffect->EndPass();
	m_pEffect->End();
	SetOldStates();
}
void CParticleRender::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}

void CParticleRender::Lost()
{
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}