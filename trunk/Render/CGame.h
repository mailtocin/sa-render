/********************************SA::Render source file*************************************/
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac                                                                   */
/* File descrip: GTA structures/enumerations/functions/defines are represented here.       */
/* File created: 17.05.2013                                                                */
/* File last ed: 09.08.2013                                                                */
/*******************************************************************************************/
#pragma once
#include "RenderWare.h"
#include "game\cDMAudio.h"
#include "game\CMenuManager.h"
#include "game\CScene.h"
#include "game\CCamera.h"
#include "game\CRect.h"
#include "game\CTimeCycleCurrent.h"
#include "game\CShadowManager.h"
#include "game\CMatrix.h"
#include "game\CPlaceable.h"
#include "game\CVector.h"
#include "game\CPad.h"

//--------------globals--------------------------------------------------
#define g_Device (*(IDirect3DDevice9 **)0xC97C28)
#define g_DirLight ((RwV3d *)0xB7CB14)
#define LastTickTime (*(unsigned int *)0xB72CA8)
#define gColourTop ((RwRGBA *)0xB72CA0)
#define LightningFlash (*(unsigned char *)0xC812CC)
#define RsGlobal ((RsGlobalType *)0xC17040)
#define g_Unk ((void *)0xC88224)
#define CreditsOn (*(unsigned char *)0xC6E97C)
#define Scene ((CScene *)0xC17038)
#define FrontEndMenuManager ((CMenuManager *)0xBA6748)
#define DMAudio ((cDMAudio *)0xB6BC90)
#define TheCamera ((CCamera *)0xB6F028)
#define Timecycle ((CTimeCycleCurrent *)0xB7C4A0)
#define ShadowManager ((CShadowManager *)0xC40350)
#define NumPointLights (*(unsigned int *)0xC3F0D0)
#define gfAspectRatio (*(float *)0xC3EFA4)
#define gfFOV (*(float *)0x8D5038)
#define gCameraSeaDepth (*(float *)0xC8132C)
#define gOcclReflectionsState (*(unsigned int *)0xC7C724)
#define byte_BB4240 ((void *)0xBB4240)
#define RwD3D9D3D9ViewTransform ((RwMatrix*) 0xC9BC80)
#define matrixBuffered ((CMatrix *)0xB6FE40)
#define TimeCycleVectorToSun ((CVector *)0xB7CA50)
#define TimeCycleCurrentStoredValue (*(unsigned int *)0xB79FD0)
#define D3D9ViewProjTransform ((RwMatrix*)0xC94C30)
#define D3D9ActiveTransform ((RwMatrix*)0xC94C70)
#define D3D9Transform ((D3DXMATRIX*)0x8D7390)
#define gMirrorsRenderingState (*(unsigned char *)0xC7C728)
#define flt_8CD4F0 (*(float *)0x8CD4F0)
#define flt_8CD4EC (*(float *)0x8CD4EC)
#define gNumCreatedHeliLights (*(unsigned int *)0xC1C96C)
#define gNumCreatedSearchlights (*(unsigned short *)0xA90830)
#define gReplayMode (*(unsigned char *)0xA43088)

#define UpdateTimer() ((void (__cdecl *)())0x561B10)()
#define InitSprite2dPerFrame() ((void (__cdecl *)())0x727350)()
#define InitFontPerFrame() ((void (__cdecl *)())0x719800)()
#define ProcessGame() ((void (__cdecl *)())0x53BEE0)()
#define ServiceDMAudio(audio) ((void (__thiscall *)(cDMAudio *))0x507750)(audio)
#define GetCameraScreenFadeStatus(camera) ((signed int (__thiscall *)(CCamera *))0x50AE20)(camera)
#define CalculateAspectRatio() ((double (__cdecl *)())0x6FF420)()
#define SetRenderWareCamera(camera) ((void (__cdecl *)(RwCamera *))0x7328C0)(camera)
#define CameraSize(camera, rect, viewWindow, aspectRatio) ((void (__cdecl *)(RwCamera *, CRect *, float, float))0x72FC70)\
	(camera, rect, viewWindow, aspectRatio)
#define GetTimeFromRenderStart() ((DWORD (__cdecl *)())0x561A80)()
#define GetTimerDivider() ((int (__cdecl *)())0x561A40)()
#define SetLightsWithTimeOfDayColour(world) ((void (__cdecl *)(RpWorld *))0x7354E0)(world)
#define CreateShadowManagerShadows(shadowManager) ((void (__thiscall *)(CShadowManager *))0x706AB0)(shadowManager)
#define RenderSceneGeometryToMirror() ((void (__cdecl *)())0x727140)()
#define DoRWStuffStartOfFrame_Horizon(topR, topG, topB, bottomR, bottomG, bottomB, A) ((unsigned int (__cdecl *)(unsigned short, unsigned short, \
	unsigned short, unsigned short, unsigned short, unsigned short, unsigned short))0x53D7A0)(topR, topG, topB, bottomR, bottomG, bottomB, A)
#define DefinedState() ((void (__cdecl *)())0x734650)()
#define RenderMirrors() ((void (__cdecl *)())0x726090)()
#define ConstructRendererRenderList() ((void (__cdecl *)())0x5556E0)()
#define PreRenderRenderer() ((void (__cdecl *)())0x553910)()
#define ProcessPedTasks() ((void (__cdecl *)())0x563430)()
#define _RenderScene() ((void (__cdecl *)())0x53DF40)()
#define RenderPedWeapons() ((void (__cdecl *)())0x732F30)()
#define sub_53E8D0(unk) ((void (__thiscall *)(void *))0x53E8D0)(unk)
#define _RenderEffects() ((void (__cdecl *)())0x53E170)()
#define SetCameraMotionBlurAlpha(camera, alpha) ((void (__thiscall *)(CCamera *, unsigned char))0x50BF80)(camera, alpha)
#define RenderCameraMotionBlur(camera) ((void (__thiscall *)(CCamera *))0x50B8F0)(camera)
#define Render2dStuff() ((int (__cdecl *)())0x53E230)()
#define DrawMenuManagerFrontEnd(menuManager) ((void (__thiscall *)(CMenuManager *))0x57C290)(menuManager)
#define DoFade() ((void (__cdecl *)())0x53E600)()
#define DrawStyledText() ((void (__cdecl *)())0x58D490)()
#define DrawMessages(priority) ((void (__cdecl *)(char))0x69EFC0)(priority)
#define DrawFonts() ((int (__cdecl *)())0x71A210)()
#define RenderCredits() ((void (__cdecl *)())0x5A87F0)()
#define DebugDisplayTextBuffer() ((void (__cdecl *)())0x532260)()
#define FlushObrsPrintfs() ((void (__cdecl *)())0x734640)()
#define DoRWRenderHorizon() ((void (__cdecl *)())0x7178F0)()
#define RenderClouds() ((void (__cdecl *)())0x713950)()
#define UpdateSunLightForCustomRenderingPipeline() ((void (__cdecl *)())0x5D5B10)()
#define RenderRoads() ((void (__cdecl *)())0x553A10)()
#define RenderCoronasReflections() ((void (__cdecl *)())0x6FB630)()
#define RenderEverythingBarRoads() ((void (__cdecl *)())0x553AA0)()
#define RenderBrokenObjects(unk1, unk2) ((void (__thiscall *)(void *, int))0x59E6A0)(unk1, unk2)
#define RenderFadingInUnderwaterEntities() ((void (__cdecl *)())0x553220)()
#define RenderWater() ((void (__cdecl *)())0x6EF650)()
#define RenderFadingInEntities() ((void (__cdecl *)())0x5531E0)()
#define sub_707F40() ((void (__cdecl *)())0x707F40)()
#define RenderStaticShadows() ((void (__cdecl *)())0x708300)()
#define RenderStoredShadows() ((void (__cdecl *)())0x70A960)()
#define RenderGrass() ((void (__cdecl *)())0x5DBAE0)()
#define sub_7154B0() ((void (__cdecl *)())0x7154B0)()
#define RenderRainStreaks() ((void (__cdecl *)())0x72AF70)()
#define RenderSunReflection() ((void (__cdecl *)())0x6FBAA0)()
#define RenderStencil() ((unsigned char (__cdecl *)())0x7113B0)()
#define MatrixSet(t, m) ((void (__thiscall *)(CMatrix *, CMatrix *))0x59BBC0)(t, m)
#define UpdateCameraFrame(t, b) ((void (__thiscall *)(CCamera *, unsigned char))0x50AFA0)(t, b)
#define SetPlaceableMatrix(t, m) ((void (__thiscall *)(CPlaceable *, CMatrix *))0x54F610)(t, m)
#define sub_5150E0(t, a, b) ((void (__thiscall *)(CCamera *, unsigned char, unsigned char))0x5150E0)(t, a, b)
#define SetCameraMirror(t) ((void (__thiscall *)(CCamera *))0x51A560)(t)
#define RemoveCameraMirror(t) ((void (__thiscall *)(CCamera *))0x51A5A0)(t)
#define FindPlayerCoors(outPoint, playerIndex) ((void (__cdecl *)(CVector *, int))0x56E010)(outPoint, playerIndex)
#define rxD3D9DefaultRenderCallback(resEntry, atomic, bClipSphere, flags) ((void (__cdecl *)(RwResEntry *, RpAtomic *, int, int)) 0x756DF0)(resEntry, atomic, bClipSphere, flags)
/* RenderEffects */
#define RenderBirds() ((void (__cdecl *)())0x712810)()
#define RenderSkidmarks() ((void (__cdecl *)())0x720640)()
#define RenderRopes() ((void (__cdecl *)())0x556AE0)()
#define RenderGlass() ((void (__cdecl *)())0x71CE20)()
#define sub_733800() ((void (__cdecl *)())0x733800)()
#define RenderCoronas() ((void (__cdecl *)())0x6FAEC0)()
#define RenderWaterCannons() ((void (__cdecl *)())0x729B30)()
#define sub_6E7760() ((void (__cdecl *)())0x6E7760)()
#define RenderCloudMasked() ((void (__cdecl *)())0x716C90)()
#define RenderHighClouds() ((void (__cdecl *)())0x716380)()
#define SetRenderStatesForSpotLights() ((void (__cdecl *)())0x6C4650)()
#define RenderHeliLights() ((void (__cdecl *)())0x6C7C50)()
#define RenderSearchlights() ((void (__cdecl *)())0x493E30)()
#define ResetRenderStatesForSpotLights() ((void (__cdecl *)())0x6C46E0)()
#define RenderWeaponEffects() ((void (__cdecl *)())0x742CF0)()
#define RenderSpecialFX() ((void (__cdecl *)())0x726AD0)()
#define RenderFogEffect() ((void (__cdecl *)())0x7002D0)()
#define RenderFirstPersonVehicle() ((void (__cdecl *)())0x553D00)()
#define RenderPostProcess() ((void (__cdecl *)())0x7046E0)()
//#define RenderParticles(ptfx, camera, unk) ((void (__thiscall *)(CFx *, RwCamera *, int))0x49E650)(ptfx, camera, unk)
//#define PtDataRenderParticles(ptdata, camera, unk) ((void (__thiscall *)(void *, RwCamera *, int))0x4A92A0)(ptdata, camera, unk)
#define GetPad(number) ((CPad *(__cdecl *)(unsigned int))0x53FB70)(number)
#define FindPlayerPed(number) ((CPed *(__cdecl *)(unsigned int))0x56E210)(number)
#define RenderWeaponTargetTriangle(ped) ((void (__thiscall *)(CPed *))0x60BA80)(ped)
/* */


// D3D Helpers
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}

bool GetSunPosn(CVector *dst);
bool GetSunPosn(CVector *dst, float farClip);
CVector *GetCamPos();
bool Im2DRenderQuad(float x1, float y1, float x2, float y2, float z, float recipCamZ, float uvOffset, unsigned int color);
D3DXMATRIX *GetWorldTransposedMatrix(D3DXMATRIX *out, RwMatrix *world);
D3DXMATRIX *getWorldViewProj(D3DXMATRIX *out, RwMatrix *world, D3DXMATRIX *viewProj);
RECT DetermineClipRect(const D3DXVECTOR3& position, const float range,D3DXMATRIX m_View,D3DXMATRIX m_Projection,float screenW,float screenH);
HRESULT GetCurrentStates(DWORD *oDB,DWORD *oSB,DWORD *oBO,DWORD *oAB,DWORD *oAT);
HRESULT SetOldStates(DWORD oDB,DWORD oSB,DWORD oBO,DWORD oAB,DWORD oAT);
class CGlobalValues
{
public:
	static D3DXVECTOR4 gm_SunPosition;
};