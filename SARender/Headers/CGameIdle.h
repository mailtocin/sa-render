/********************************SA::Render source file*************************************/
/* File name   : CGameIdle.h                                                               */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CGameIdle is replacement for standard Idle function.                      */
/* File created: 12.05.2014                                                                */
/* File last ed: 12.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CGame.h"
#include "CRTMgr.h"
class CGameIdle
{
public:
	static void Patch();
	static void Initialize();
	static void RenderReflections(CVector vPositon, int id);
	static void RenderScene();
	static void Idle(void *Data);
	static RwCamera* m_pReflCam;
	static RwFrame* m_pReflFrame;
	static CRenderTarget *m_pReflectionRT[3];
	static bool m_bUseAlphaTestForTexAlpha;
};
extern bool g_bDrawGUI;
