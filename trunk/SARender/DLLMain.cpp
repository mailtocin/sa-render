/********************************SA::Render source file*************************************/
/* File name   : DLLMain.cpp                                                               */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: Main.                                                                     */
/* File created: 11.05.2014                                                                */
/* File last ed: 29.07.2014                                                                */
/*******************************************************************************************/
#include <Windows.h>
#include "plugin\plugin.h"
#include "patch\CPatch.h"
#include "CGTAVTimeCycle.h"
#include "Headers\CVehicleDrawable.h"
#include "Headers\CBuildingDrawable.h"
#include "Headers\CNormalsGenerator.h"
#include "Headers\CObjectDrawable.h"
#include "Headers\CGameIdle.h"
#include "Headers\CEffectMgr.h"
#include "Headers\CGUI.h"
#include "Headers\CRTMgr.h"
#include "Headers\CShadowMgr.h"
#include "Headers\CDeferredMgr.h"
#include "Headers\CSkinDrawable.h"
#include "Headers\CDebug.h"
#include "Headers\CLight.h"

using namespace plugin;

void BeforeReset()
{
	gEffectMgr.OnLostDevice();
	gRTMgr.Lost();
	TwWindowSize(0, 0);
	CDeferredMgr::ReInitialize();
}

void AfterReset()
{
	gEffectMgr.OnResetDevice();
	gRTMgr.Reset();
	TwWindowSize(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight);
}

void Shutdown()
{
	CVehicleDrawable::Shutdown();
	CBuildingDrawable::Shutdown();
	CObjectDrawable::Shutdown();
	CDeferredMgr::ShutDown();
	gGUI.Shutdown();
	gEffectMgr.Shutdown();
	gRTMgr.Shutdown();
}

void Initialize()
{
	gEffectMgr.Initialise();
	//gRTMgr.Initialise();
	gGUI.Initialize();
	CVehicleDrawable::Initialize();
	CDebug::Initialize();
	CBuildingDrawable::Initialize();
	CObjectDrawable::Initialize();
	CShadowMgr::Initialize();
	CDeferredMgr::Initialize();
	CGameIdle::Initialize();
}

void Patch()
{
	//
	CPatch::RedirectCall(0x5BD765, Initialize);
	CPatch::SetInt(0x53E1A5, 0xC3);
	CNormalsGenerator::Patch();
	CVehicleDrawable::Patch();
	CBuildingDrawable::Patch();
	CObjectDrawable::Patch();
	CSkinDrawable::Patch();
	CGameIdle::Patch();
	CLights::Patch();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CGTAVTimeCycle::ParseTC();
		System::RegisterPlugin("SA::Render", "DK22Pac, PetkaGtA", "SA_Render.asi", "0.03d", 2, GAME_SA_1_0_US, NULL);
		Core::RegisterFunc(FUNC_BEFORE_RESET, BeforeReset);
		Core::RegisterFunc(FUNC_AFTER_RESET, AfterReset);
		Patch();
		break;
	case DLL_PROCESS_DETACH:
		Shutdown();
		break;
	default:
		break;
	}
	return TRUE;
}