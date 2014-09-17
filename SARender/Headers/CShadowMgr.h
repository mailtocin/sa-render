/********************************SA::Render source file*************************************/
/* File name   : CShadowMgr.h                                                              */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: Controls Shadow Mapping.                                                  */
/* File created: 15.05.2014                                                                */
/* File last ed: 15.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CEffect.h"
#include "CRenderTarget.h"
#include "CGame.h"

enum eSunShadowType
{
	ST_STANDARD
};
enum eSunShadowCenteredOn
{
	SCO_FUSTRUM,
	SCO_PLAYER,
	SCO_CAMERA
};
enum eShadowRenderType
{
	SRT_DYNAMIC,
	SRT_STATIC,
	SRT_BOTH
};
class CShadowMgr
{
public:
	static void Initialize();
	static void CalculateMatrix(int iMatrixID);
	static void Render(eShadowRenderType RenderType, float fDistance, int iMatrixID);
	static void Update();
	static void SetShadowParamsToShader(CEffect *pEffect);
	static CEffect *m_pEffect;
	static D3DXMATRIX g_mLightViewProj[2];
	static D3DXMATRIX g_mLightView[2];
	static CRenderTarget *m_pShadowRT[2];
	static CRenderTarget *m_pShadowDepthRT[2];
	static eSunShadowCenteredOn m_dwSunShadowCenterOn;
	static float m_fMaxDistance;
	static float m_fMaxDistanceToRender;
	static bool m_bRenderDayShadows;
	static bool m_bRenderNightShadows;
	static int m_nShadowSize;
};
