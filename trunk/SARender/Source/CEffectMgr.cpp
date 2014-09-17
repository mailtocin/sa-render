/********************************SA::Render source file*************************************/
/* File name   : CEffectMgr.cpp                                                            */
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac                                                                   */
/* File descrip: CEffectMgr controls created CEffect objects.                              */
/* File created: 01.12.2013                                                                */
/* File last ed: 02.12.2013                                                                */
/*******************************************************************************************/
#include "..\Headers\CEffectMgr.h"
//#include "plugin\plugin.h"

CEffectMgr::CEffectMgr()
{
	m_bInitialised = false;
	m_dwNumEffects = 0;
	m_pEffectPool = NULL;
}

void CEffectMgr::Initialise()
{

	HRESULT hr = D3DXCreateEffectPool(&m_pEffectPool);
	if (FAILED(hr))
	{
		MessageBox(0, "D3DXCreateEffectPool() - FAILED", "CEffectMgr::Initialise", 0);
		m_pEffectPool = NULL;
	}
	else
		m_bInitialised = true;
}

void CEffectMgr::Reload()
{
	if (!m_bInitialised)
		return;
	for (auto i = m_effects.begin(); i != m_effects.end(); ++i)
		i._Ptr->_Myval->Reload();
}

void CEffectMgr::Shutdown()
{
	if (m_bInitialised)
	{
		if (m_effects.size() != 0)
			MessageBox(0, "CEffectMgr::m_effects is not empty!", "CEffectMgr::Shutdown()", 0);
		if (m_pEffectPool)
		{
			m_pEffectPool->Release();
			m_pEffectPool = NULL;
		}
		m_bInitialised = false;
	}
}

void CEffectMgr::OnLostDevice()
{
	if (!m_bInitialised)
		return;
	for (auto i = m_effects.begin(); i != m_effects.end(); ++i)
		i._Ptr->_Myval->OnLost();
}

void CEffectMgr::OnResetDevice()
{
	if (!m_bInitialised)
		return;
	for (auto i = m_effects.begin(); i != m_effects.end(); ++i)
		i._Ptr->_Myval->OnReset();
}

void CEffectMgr::SetGlobalMatrix(D3DXHANDLE handle,D3DXMATRIX *value)
{
	if (!m_bInitialised)
		return;
	for (auto i = m_effects.begin(); i != m_effects.end(); ++i)
		i._Ptr->_Myval->SetMatrix(handle, value);
}

void CEffectMgr::AddEffect(CEffect *effect)
{
	if (m_bInitialised)
	{
		m_effects.push_back(effect);
	}
}

void CEffectMgr::RemoveEffect(CEffect *effect)
{
	m_effects.remove(effect);
}

CEffectMgr gEffectMgr;
eRenderState gRenderState;