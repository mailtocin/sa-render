/********************************SA::Render source file*************************************/
/* File name   : CEffectMgr.h                                                              */
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac, PetkaGtA                                                         */
/* File descrip: CEffectMgr controls created CEffect objects.                              */
/* File created: 01.12.2013                                                                */
/* File last ed: 01.12.2013                                                                */
/*******************************************************************************************/
#pragma once
#include "CEffect.h"
#include <list>
enum eRenderState
{
	RENDERTYPE_DEFERRED,
	RENDERTYPE_SHADOW,
	RENDERTYPE_PLSHADOW,
	RENDERTYPE_FORWARD,
	RENDERTYPE_VELOCITY,
	RENDERTYPE_REFL
};
class CEffectMgr
{
	unsigned int m_dwNumEffects;
	std::list<CEffect*> m_effects;
public:
	CEffectMgr();
	bool m_bInitialised;
	ID3DXEffectPool *m_pEffectPool;
	void Initialise();
	void Reload();
	void Shutdown();
	void OnLostDevice();
	void OnResetDevice();
	void SetGlobalMatrix(D3DXHANDLE handle,D3DXMATRIX *value);
	void AddEffect(CEffect *effect);
	void RemoveEffect(CEffect *effect);
};

extern CEffectMgr gEffectMgr;
extern eRenderState gRenderState;