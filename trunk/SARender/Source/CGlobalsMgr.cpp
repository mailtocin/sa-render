/********************************SA::Render source file*************************************/
/* File name   : CGlobalsMgr.cpp                                                           */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CGlobalsMgr contains global matricles and vectors.                        */
/* File created: 12.05.2014                                                                */
/* File last ed: 13.05.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CGlobalsMgr.h"

void CGlobalsMgr::Initialize()
{
	// Get Viewport Dimensions
	g_fInvViewportDimensions = D3DXVECTOR2(1.0f / (float)RsGlobal->MaximumWidth, 1.0f / (float)RsGlobal->MaximumHeight);
	// Get Positions.
	GetSunPosn((CVector*)&g_vSunPosition, Timecycle->m_fCurrentFarClip);
	FindPlayerCoors((CVector*)&g_vPlayerPosition, 0);
	g_vCameraPosition = *(D3DXVECTOR4*)GetCamPos();
	// Get Matricles.
	g_Device->GetTransform(D3DTS_VIEW, &g_mView);
	g_Device->GetTransform(D3DTS_PROJECTION, &g_mProj);
	D3DXMatrixMultiply(&g_mViewProj, &g_mView, &g_mProj);
	// Inverse them.
	D3DXMatrixInverse(&g_mViewInv, NULL, &g_mView);
	D3DXMatrixInverse(&g_mProjInv, NULL, &g_mProj);
	D3DXMatrixInverse(&g_mViewProjInv, NULL, &g_mViewProj);
}

void CGlobalsMgr::ReInitViewProj()
{
	// Get Matricles.
	g_Device->GetTransform(D3DTS_VIEW, &g_mView);
	g_Device->GetTransform(D3DTS_PROJECTION, &g_mProj);
	D3DXMatrixMultiply(&g_mViewProj, &g_mView, &g_mProj);
	// Inverse them.
	D3DXMatrixInverse(&g_mViewInv, NULL, &g_mView);
	D3DXMatrixInverse(&g_mProjInv, NULL, &g_mProj);
	D3DXMatrixInverse(&g_mViewProjInv, NULL, &g_mViewProj);
}

void CGlobalsMgr::ReInitViewProj(D3DXMATRIX *mView, D3DXMATRIX *mProj)
{
	// Set Matricles.
	g_Device->SetTransform(D3DTS_VIEW, mView);
	g_Device->SetTransform(D3DTS_PROJECTION, mProj);
	// Calculate View-Projection Matrix.
	D3DXMatrixMultiply(&g_mViewProj, mView, mProj);
	// Inverse them.
	D3DXMatrixInverse(&g_mViewInv, NULL, mView);
	D3DXMatrixInverse(&g_mProjInv, NULL, mProj);
	D3DXMatrixInverse(&g_mViewProjInv, NULL, &g_mViewProj);
}

void CGlobalsMgr::GetViewProjRef()
{
	// Get Matricles.
	g_Device->GetTransform(D3DTS_VIEW, &g_mOldView);
	g_Device->GetTransform(D3DTS_PROJECTION, &g_mOldProj);
}

void CGlobalsMgr::ResetViewProjRef()
{
	// Set Matricles.
	g_Device->SetTransform(D3DTS_VIEW, &g_mOldView);
	g_Device->SetTransform(D3DTS_PROJECTION, &g_mOldProj);
}

void CGlobalsMgr::SetGlobalMatrixToEffect(CEffect* Effect, D3DXMATRIX *mMatrix, eGlobalsType GlobalsType){
	switch (GlobalsType)
	{
	case GT_World:
		Effect->SetMatrix(Effect->m_globals.globals.WorldMatrix, mMatrix);
		break;
	case GT_View:
		Effect->SetMatrix(Effect->m_globals.globals.ViewMatrix, mMatrix);
		break;
	case GT_Proj:
		break;
	case GT_ViewProj:
		break;
	case GT_WorldViewProj:
		Effect->SetMatrix(Effect->m_globals.globals.WorldViewMatrix, mMatrix);
		break;
	case GT_WorldView:
		break;
	case GT_LightViewProj:
		Effect->SetMatrix(Effect->m_globals.globals.LightViewProjMatrix, mMatrix);
		break;
	default:
		break;
	}
	
}

CGlobalsMgr gGlobalsMgr;