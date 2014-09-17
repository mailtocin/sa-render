/********************************SA::Render source file*************************************/
/* File name   : CRTMgr.h																   */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CRTMgr is render target controller.								       */
/* File created: 13.05.2014                                                                */
/* File last ed: 13.05.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CRTMgr.h"
#include "..\Headers\CRenderTarget.h"

CRTMgr gRTMgr;

void CRTMgr::Initialise()
{
	m_pOldSurf = NULL;
}

void CRTMgr::AddRT(CRenderTarget *pRT)
{
	m_RTs.push_back(pRT);
}

void CRTMgr::RemoveRT(CRenderTarget *pRT)
{
	m_RTs.remove(pRT);
}

void CRTMgr::Shutdown()
{
	for (auto i = m_RTs.begin(); i != m_RTs.end(); ++i)
		delete (*i);
}

void CRTMgr::Lost()
{
	for (auto i = m_RTs.begin(); i != m_RTs.end(); ++i)
	{
		(*i)->OnLostDevice();
	}
}

void CRTMgr::Reset()
{
	for (auto i = m_RTs.begin(); i != m_RTs.end(); ++i)
	{
		(*i)->OnResetDevice();
	}
}

void CRTMgr::GetRef()
{
	g_Device->GetRenderTarget(0, &m_pOldSurf);
	m_pOldSurf->Release();
}
void CRTMgr::ResetRef()
{
	g_Device->SetRenderTarget(0, m_pOldSurf);
	g_Device->SetRenderTarget(1, 0);
	g_Device->SetRenderTarget(2, 0);
	g_Device->SetRenderTarget(3, 0);
}
void CRTMgr::GetZRef()
{
	g_Device->GetDepthStencilSurface(&m_pOldZSurf);
	m_pOldZSurf->Release();
}
void CRTMgr::ResetZRef()
{
	g_Device->SetDepthStencilSurface(m_pOldZSurf);
	SAFE_RELEASE(m_pOldZSurf);
}