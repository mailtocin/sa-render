/********************************SA::Render source file*************************************/
/* File name   : CGlobalsMgr.h                                                             */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CGlobalsMgr contains global matricles and vectors.                        */
/* File created: 12.05.2014                                                                */
/* File last ed: 12.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CGame.h"
#include "CEffectMgr.h"
enum eGlobalsType{
	GT_World,
	GT_View,
	GT_Proj,
	GT_ViewProj,
	GT_WorldViewProj,
	GT_WorldView,
	GT_LightViewProj
};
class CGlobalsMgr
{
public:
	void Initialize();
	void ReInitViewProj();
	void ReInitViewProj(D3DXMATRIX *mView, D3DXMATRIX *mProj);
	void GetViewProjRef();
	void ResetViewProjRef();
	void SetGlobalMatrixToEffect(CEffect* Effect, D3DXMATRIX *Matrix, eGlobalsType GlobalsType);
private:
	D3DXMATRIX g_mOldView, g_mOldProj;
public:
	D3DXVECTOR4 g_vSunPosition;
	D3DXVECTOR4 g_vPlayerPosition;
	D3DXVECTOR4 g_vCameraPosition;
	D3DXVECTOR2 g_fInvViewportDimensions;
	D3DXMATRIX g_mView, g_mProj;
	D3DXMATRIX g_mViewInv, g_mProjInv;
	D3DXMATRIX g_mViewProj, g_mViewProjInv;
	float g_fReflDir;
	int g_iResourceTXD;
	int g_iReflTexSize;
};
extern CGlobalsMgr gGlobalsMgr;
