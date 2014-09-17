/********************************SA::Render source file*************************************/
/* File name   : CGameIdle.cpp                                                             */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CGameIdle is replacement for standard Idle function.                      */
/* File created: 12.05.2014                                                                */
/* File last ed: 25.07.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CGameIdle.h"
#include "..\Headers\AntTweakBar.h"
#include "patch\CPatch.h"
#include "..\Headers\CGlobalsMgr.h"
#include "..\Headers\CEffectMgr.h"
#include "..\Headers\CRenderTarget.h"
#include "..\Headers\CShadowMgr.h"
#include "..\Headers\CDeferredMgr.h"
#include "..\Headers\CDebug.h"
#include "..\Headers\CLight.h"
#include "..\Headers\CGUI.h"

bool g_bDrawGUI;

HHOOK hookHandle;

LRESULT CALLBACK MessageProc(int code, WPARAM wParam, LPARAM lParam) {
	if (lParam & 0x80000000 || lParam & 0x40000000)
	{
		return CallNextHookEx(hookHandle, code, wParam, lParam);
	}

	if (code >= 0){
		if (TwEventWin(((LPMSG)lParam)->hwnd, ((LPMSG)lParam)->message, ((LPMSG)lParam)->wParam, ((LPMSG)lParam)->lParam)){
			return FALSE;
		}
	}
	return CallNextHookEx(hookHandle, code, wParam, lParam);
}

RwCamera *CGameIdle::m_pReflCam;
RwFrame *CGameIdle::m_pReflFrame;
CRenderTarget *CGameIdle::m_pReflectionRT[3];
__int64 Freq;
__int64 Start;
__int64 Now;
double fReflTime;
double fGBuffTime;
double fDeferredTime;
double fShadowTime;
void nullsub(void*){

}
RpWorld* InitWorld(RwBBox*bbox){
	bbox->inf.x *= 2;
	bbox->inf.y *= 2;
	bbox->inf.z *= 2;
	bbox->sup.x *= 2;
	bbox->sup.y *= 2;
	bbox->sup.z *= 2;
	return RpWorldCreate(bbox);
}
void CGameIdle::Patch()
{
	g_bDrawGUI = false;
	CPatch::RedirectCall(0x53ECBD, Idle);
	CPatch::RedirectCall(0x5BD71C, InitWorld);
	
	CPatch::RedirectCall(0x53C1AB, nullsub);
	SetWindowsHookEx(WH_GETMESSAGE, MessageProc, NULL, GetCurrentThreadId());
}

void CGameIdle::Initialize(){
	gGlobalsMgr.g_iReflTexSize = 8;
	for (int i = 0; i < 2; i++)
		m_pReflectionRT[i] = new CRenderTarget(gGlobalsMgr.g_iReflTexSize, gGlobalsMgr.g_iReflTexSize, 0, D3DFMT_A8R8G8B8, false);
	m_pReflCam = RwCameraCreate();
	if (m_pReflCam){
		m_pReflFrame = RwFrameCreate();
		m_pReflCam->frameBuffer = RwRasterCreate(gGlobalsMgr.g_iReflTexSize, gGlobalsMgr.g_iReflTexSize, 24, 5);
		m_pReflCam->zBuffer = RwRasterCreate(gGlobalsMgr.g_iReflTexSize, gGlobalsMgr.g_iReflTexSize, 24, 1);
		CameraSize(m_pReflCam, 0, tan(D3DX_PI / 2), 1.0f);
		RwObjectHasFrameSetFrame(&m_pReflCam->object.object, m_pReflFrame);
		RpWorldAddCamera(Scene->m_pRpWorld, m_pReflCam);
	}
	gGUI.AddVar("Reflection Time", TW_TYPE_DOUBLE, &fReflTime);
	gGUI.AddVar("GBuffer Time", TW_TYPE_DOUBLE, &fGBuffTime);
	gGUI.AddVar("Deferred Time", TW_TYPE_DOUBLE, &fDeferredTime);
	gGUI.AddVar("Shadow Time", TW_TYPE_DOUBLE, &fShadowTime);
}

void CGameIdle::RenderReflections(CVector vPositon,int id){
	D3DXMATRIX mView, mProj;
	CVector vAxisA, vAxisB;
	float fAngleA, fAngleB;

	// Render To Cubemap.
	vAxisA = CVector(0, 1, 0);
	vAxisB = CVector(1, 0, 0);
	fAngleA = 180.0;
	fAngleB = 90.0;
	if (id == 0)
		gGlobalsMgr.g_fReflDir = 1;
	else
		gGlobalsMgr.g_fReflDir = -1;

	// Setup position.
	RwFrameRotate(m_pReflFrame, (RwV3d*)&vAxisA, fAngleA, rwCOMBINEREPLACE);
	RwFrameRotate(m_pReflFrame, (RwV3d*)&vAxisB, fAngleB, rwCOMBINEPOSTCONCAT);
	RwFrameTranslate(m_pReflFrame, (RwV3d*)&vPositon, rwCOMBINEPOSTCONCAT);

	// Init camera parameters and clear Z-Buffer.
	RwCameraClear(m_pReflCam, gColourTop, rwCAMERACLEARZ);
	RwCameraSetFarClipPlane(m_pReflCam, 350);
	
	// Set Reflection RT and Clear it.
	m_pReflectionRT[id]->SetRenderTarget(0);
	g_Device->Clear(0, 0, 3, D3DXCOLOR((float)Timecycle->m_nCurrentSkyTopRed / 255.0f, (float)Timecycle->m_nCurrentSkyTopGreen / 255.0f, (float)Timecycle->m_nCurrentSkyTopBlue / 255.0f, 1.0), 1.0, 0);

	// Render To Paraboloid map.
	gRenderState = RENDERTYPE_REFL;
	RwCameraBeginUpdate(m_pReflCam);
	gGlobalsMgr.ResetViewProjRef();
	RenderRoads();
	RenderEverythingBarRoads();
	RwCameraEndUpdate(m_pReflCam);
	gRenderState = RENDERTYPE_FORWARD;
}
void CGameIdle::RenderScene(){
	RenderRoads();
	RenderEverythingBarRoads();         // CRenderer::RenderEverythingBarRoads
	RenderFadingInUnderwaterEntities(); // CRenderer::RenderFadingInUnderwaterEntities
	RenderFadingInEntities();
}
void CGameIdle::Idle(void *Data)
{
	RwV2d mousePos;
	UpdateTimer();
	CSprite2d::InitPerFrame();
	InitFontPerFrame();
	NumPointLights = 0;
	CLights::m_nNumLights = 0;
	ProcessGame();
	ServiceDMAudio(DMAudio);
	SetLightsWithTimeOfDayColour(Scene->m_pRpWorld);
	if (!Data)
		return;
	if (FrontEndMenuManager->m_bMenuActive || GetCameraScreenFadeStatus(TheCamera) == 2) {
		CalculateAspectRatio(); // CDraw::CalculateAspectRatio
		CameraSize(Scene->m_pRwCamera, 0, tan(gfFOV * 0.0087266462f), gfAspectRatio);
		SetRenderWareCamera(Scene->m_pRwCamera); // CVisibilityPlugins::SetRenderWareCamera
		RwCameraClear(Scene->m_pRwCamera, gColourTop, 2);
		if (!RsCameraBeginUpdate(Scene->m_pRwCamera))
			return;
	}
	else
	{
//***********************************************************************************
		POINT pMousePos;
		g_bDrawGUI = GetKeyState(VK_F12) & 1;
		GetCursorPos(&pMousePos);
//***********************************************************************************

		mousePos.x = RsGlobal->MaximumWidth * 0.5f;
		mousePos.y = RsGlobal->MaximumHeight * 0.5f;

//***********************************************************************************
		if (!g_bDrawGUI)
			RsMouseSetPos(&mousePos);
		else
			TwMouseMotion(pMousePos.x, pMousePos.y);
//***********************************************************************************
		ConstructRendererRenderList(); // CRenderer::ConstructRenderList
		PreRenderRenderer();           // CRenderer::PreRender
		ProcessPedTasks();

		QueryPerformanceCounter((PLARGE_INTEGER)&Start);
		QueryPerformanceFrequency((PLARGE_INTEGER)&Freq);

		RenderReflections(*(CVector*)&gGlobalsMgr.g_vPlayerPosition, 0);
		RenderReflections(*(CVector*)&gGlobalsMgr.g_vPlayerPosition, 1);

		QueryPerformanceCounter((PLARGE_INTEGER)&Now);
		fReflTime = ((double(Now - Start)) / (double)Freq) * 1000;

		if (LightningFlash) // CWeather::LightningFlash
		{
			Timecycle->m_nCurrentSkyBottomRed = 255;
			Timecycle->m_nCurrentSkyBottomGreen = 255;
			Timecycle->m_nCurrentSkyBottomBlue = 255;
			if (!DoRWStuffStartOfFrame_Horizon(255, 255, 255, 255, 255, 255, 255))
				return;
		}
		else if (!DoRWStuffStartOfFrame_Horizon(Timecycle->m_nCurrentSkyTopRed, Timecycle->m_nCurrentSkyTopGreen, Timecycle->m_nCurrentSkyTopBlue,
			Timecycle->m_nCurrentSkyBottomRed, Timecycle->m_nCurrentSkyBottomGreen, Timecycle->m_nCurrentSkyBottomBlue, 255))
			return;
		
		DefinedState();
		RwCameraSetFarClipPlane(Scene->m_pRwCamera, Timecycle->m_fCurrentFarClip);
		Scene->m_pRwCamera->fogPlane = Timecycle->m_fCurrentFogStart;
		gGlobalsMgr.Initialize();

		QueryPerformanceCounter((PLARGE_INTEGER)&Start);
		QueryPerformanceFrequency((PLARGE_INTEGER)&Freq);

		CShadowMgr::Update();

		QueryPerformanceCounter((PLARGE_INTEGER)&Now);
		fShadowTime = ((double(Now - Start)) / (double)Freq) * 1000;

		CShadowMgr::SetShadowParamsToShader(CDeferredMgr::m_pEffect);

		QueryPerformanceCounter((PLARGE_INTEGER)&Start);
		QueryPerformanceFrequency((PLARGE_INTEGER)&Freq);

		CDeferredMgr::RenderToGeometryBuffer(RenderScene);

		QueryPerformanceCounter((PLARGE_INTEGER)&Now);
		fGBuffTime = ((double(Now - Start)) / (double)Freq) * 1000;

		QueryPerformanceCounter((PLARGE_INTEGER)&Start);
		QueryPerformanceFrequency((PLARGE_INTEGER)&Freq);

		CDeferredMgr::RenderToScreenOutput();

		QueryPerformanceCounter((PLARGE_INTEGER)&Now);
		fDeferredTime = ((double(Now - Start)) / (double)Freq)*1000;
		RwD3D9RenderStateReset();
		DefinedState();
		_RenderEffects();
		Render2dStuff();
		
//***********************************************************************************
		RwD3D9RenderStateReset();
		ShowCursor(g_bDrawGUI);
		if (g_bDrawGUI)
			TwDraw();
//***********************************************************************************
	}
	if (FrontEndMenuManager->m_bMenuActive)
		DrawMenuManagerFrontEnd(FrontEndMenuManager); // CMenuManager::DrawFrontEnd
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
	//---------------------------Text---------------------------------------
	DoFade();
	DrawStyledText();
	DrawMessages(0); // CMessages::Display
	DrawFonts();     // CFont::DrawFonts
	if (CreditsOn)
	{
		if (!FrontEndMenuManager->m_bMenuActive)
			RenderCredits();       // CCredits::Render
		if (CreditsOn)
		{
			if (!FrontEndMenuManager->m_bMenuActive)
				RenderCredits();   // CCredits::Render
		}
	}
	DebugDisplayTextBuffer();
	FlushObrsPrintfs();
	RwCameraEndUpdate(Scene->m_pRwCamera);
	RsCameraShowRaster(Scene->m_pRwCamera);
}
