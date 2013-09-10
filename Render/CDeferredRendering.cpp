#include "CDeferredRendering.h"
#include "CPatch.h"
#include "CDebug.h"
#include "CVehicleRender.h"
#include "CObjectRender.h"
#include "CPedsRender.h"
#include "CImmediateRender.h"
#include "CParticleRender.h"
#include "CWaterRender.h"
#include "CLights.h"
#include "CSkyRender.h"
#include "CPostProcess.h"
#include <stdio.h>
#include <sstream>

// Shadows textures/surfaces
IDirect3DTexture9 *CDeferredRendering::shadow[CDeferredRendering::CascadeCount*2];
IDirect3DSurface9 *CDeferredRendering::shadowSurface[CDeferredRendering::CascadeCount*2];
// Geometry Buffer textures/surfaces
IDirect3DTexture9 *CDeferredRendering::gbuffer[3];
IDirect3DSurface9 *CDeferredRendering::gbSurface[3];
// Temporary Post-Processing textures/surfaces
IDirect3DTexture9 **CDeferredRendering::rtTmpSurface;
IDirect3DSurface9 **CDeferredRendering::rsTmpSurface;
// Lighting Buffer textures/surfaces
IDirect3DTexture9 *CDeferredRendering::lightingTexture;
IDirect3DSurface9 *CDeferredRendering::lightingSurface;
// Planar Reflection textures/surfaces
IDirect3DTexture9 *CDeferredRendering::reflectionTexture;
IDirect3DSurface9 *CDeferredRendering::reflectionSurface;
// Post-Process textures/surfaces count
int CDeferredRendering::ppTCcount;
// Post-Process textures/surfaces width/height
int *CDeferredRendering::surfWidth;
int *CDeferredRendering::surfHeight;
// Shadow View/Projection Matrix
D3DXMATRIX CDeferredRendering::g_mLightView[4];
D3DXMATRIX CDeferredRendering::g_mLightProj[4];
// Reflection ViewProjection Matrix
D3DXMATRIX CDeferredRendering::g_mReflViewProj;
// Deferred/Post-Process Shader
ID3DXEffect *CDeferredRendering::m_pEffect;
// Noise texture for Post-Process
IDirect3DTexture9 *noise;
// Cubemap for Reflections
IDirect3DCubeTexture9 *CDeferredRendering::cubemap = NULL;
// D3D parameters
D3DPRESENT_PARAMETERS *g_D3Dpp = (D3DPRESENT_PARAMETERS *) 0xC9C040;
// Shadow stuff
float m_fExtraDistance;
D3DXVECTOR3 *m_vUpVector;
D3DXVECTOR3 m_vLightDirection;
float m_faSplitDistances[CDeferredRendering::CascadeCount+1];
float CDeferredRendering::maxShadowDistance = 1500;
int CDeferredRendering::ShadowMapSize = 4096;
float CDeferredRendering::ShadowBias = 0.0005f;
D3DXVECTOR3 m_vaFrustumCorners[8];

// Setup function
bool CDeferredRendering::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result;
	m_vUpVector = new D3DXVECTOR3(0, 0, 1);
	// Loading shader... TODO: Load it from folder...
	result = D3DXCreateEffectFromFile(g_Device,"resources/Shaders/deferred.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CDeferredRendering", "deferred", result))	{
		return false;
	}
//-------------------------Loading textures-------------------------------
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/noise.png",&noise);
	D3DXCreateCubeTextureFromFile(g_Device,"resources/Textures/grace_diffuse_cube.dds",&cubemap);
//------------------------------------------------------------------------
	char cStr[256];
	char cPath[MAX_PATH];
	GetModuleFileName(NULL, cPath, MAX_PATH);
	if (strrchr(cPath, '\\')) *(char*)(strrchr(cPath, '\\') + 1) = '\0';
	strcat_s(cPath, MAX_PATH, "resources/SARender.ini");
	GetPrivateProfileString("SHADOWS", "MaxShadowDistance", "1500", cStr, 256, cPath);
	maxShadowDistance = (float)atof(cStr);
	GetPrivateProfileString("SHADOWS", "ShadowMapSize", "4096", cStr, 256, cPath);
	ShadowMapSize = (int)atoi(cStr);
	GetPrivateProfileString("SHADOWS", "ShadowBias", "0.0005", cStr, 256, cPath);
	ShadowBias = (float)atof(cStr);

//------------------------Post-Process stuff------------------------------
	// 1) We need to get postprocessing techniques count.
	m_pEffect->GetInt("PostProcessCount",&ppTCcount);
	// 2) We need to allocate surface/texture arrays.
	rtTmpSurface = (IDirect3DTexture9**)calloc(ppTCcount,sizeof(IDirect3DTexture9*));
	rsTmpSurface = (IDirect3DSurface9**)calloc(ppTCcount,sizeof(IDirect3DSurface9*));
	surfWidth = (int*)calloc(ppTCcount,sizeof(int));
	surfHeight = (int*)calloc(ppTCcount,sizeof(int));
	// 3) We need to set sizes of textures
	m_pEffect->SetInt("ScreenSizeX",RsGlobal->MaximumWidth);
	m_pEffect->SetInt("ScreenSizeY",RsGlobal->MaximumHeight);
	m_pEffect->SetInt("CascadeCount",CDeferredRendering::CascadeCount);
	if(ppTCcount>1){
		for(int i = 0; i<ppTCcount;i++){
			std::stringstream sstm1;
			sstm1 << "screenBuffer_" << i;
			std::string result1 = sstm1.str();
			m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),0),&surfWidth[i]);
			m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),1),&surfHeight[i]);
		}
	}
	std::stringstream sstm1;
	sstm1 << "screenBuffer_" << 0;
	std::string result1 = sstm1.str();
	m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),0),&surfWidth[0]);
	m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),1),&surfHeight[0]);
//------------------------------------------------------------------------
	return true;
}



// On Reset Device
void CDeferredRendering::Reset()
{

	if(m_pEffect)
		m_pEffect->OnResetDevice();
}


//--------------------------On Lost Device--------------------------------
void CDeferredRendering::Lost()
{
	for(int i = 0;i<CascadeCount*2;i++) {
		SAFE_RELEASE(shadowSurface[i]);
		SAFE_RELEASE(shadow[i]);
	}
	for(int i = 0;i<3;i++) {
		SAFE_RELEASE(gbSurface[i]);
		SAFE_RELEASE(gbuffer[i]);
	}
	SAFE_RELEASE(lightingSurface);
	SAFE_RELEASE(lightingTexture);
	//SAFE_RELEASE(cubemap);
	if(ppTCcount>0){
		for(int i = 0; i<ppTCcount;i++){
			SAFE_RELEASE(rsTmpSurface[i]);
			SAFE_RELEASE(rtTmpSurface[i]);
		}
	}
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}
//------------------------------------------------------------------------

//---------------------------Patch Function-------------------------------
void CDeferredRendering::Patch()
{
	CPatch::RedirectCall(0x53ECBD, Idle);
	CPatch::SetInt(0x53E1A5, 0xC3);
}
//------------------------------------------------------------------------

//-------------------------Post-Process loop------------------------------
void CDeferredRendering::PostProcess(IDirect3DSurface9 *outSurf){
	std::string tempString;
	for(int i = 1; i<ppTCcount;i++){
		g_Device->SetRenderTarget(0,rsTmpSurface[i]);

		tempString = "PostProcess_";
		tempString += std::to_string((long double)i - 1);
		m_pEffect->SetTechnique((char*)tempString.c_str());
		tempString = "screenBuffer_";
		tempString += std::to_string((long double)i - 1);
		m_pEffect->SetTexture((char*)tempString.c_str(),rtTmpSurface[i-1]);
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);
		DrawPostProcessPass(m_pEffect);
	}
	g_Device->SetRenderTarget(0,outSurf);
	tempString = "PostProcess_";
	tempString += std::to_string((long double)ppTCcount - 1);
	m_pEffect->SetTechnique((char*)tempString.c_str());

	tempString = "screenBuffer_";
	tempString += std::to_string((long double)ppTCcount - 1);
	m_pEffect->SetTexture((char*)tempString.c_str(),rtTmpSurface[ppTCcount-1]);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);
	DrawPostProcessPass(m_pEffect);
}
//------------------------------------------------------------------------

void CDeferredRendering::DrawWaterReflection(){
	IDirect3DSurface9* pOldRTSurf= NULL;
	D3DXMATRIX mOldView,proj;
	D3DXMATRIX mView, mReflect;
	g_Device->GetTransform(D3DTS_VIEW,&mOldView);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	g_Device->GetRenderTarget(0, &pOldRTSurf);
	pOldRTSurf->Release();
	CVector camPos,*cam;
	cam = GetCamPos();
	FindPlayerCoors(&camPos, 0);
	D3DXPLANE plane;
	D3DXPlaneFromPointNormal(&plane,&D3DXVECTOR3(camPos.x,camPos.y,camPos.z),&D3DXVECTOR3(camPos.x,camPos.y+1,camPos.z));
	D3DXMatrixLookAtLH(&mView,&D3DXVECTOR3(cam->x,cam->y,cam->z),&D3DXVECTOR3(camPos.x,camPos.y,camPos.z),&D3DXVECTOR3(0,1,0));
	//D3DXMatrixReflect( &mReflect, &plane );
    //D3DXMatrixMultiply( &mView, &mReflect, &mOldView );
	D3DXMatrixMultiplyTranspose(&g_mReflViewProj,&mView,&proj);
	g_Device->SetTransform(D3DTS_VIEW,&mView);
	g_Device->SetRenderTarget(0,reflectionSurface);
	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	CObjectRender::m_pEffect->SetTechnique("Reflection");
	RenderEverythingBarRoads();
	g_Device->SetRenderTarget(0,pOldRTSurf);
	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	g_Device->SetTransform(D3DTS_VIEW,&mOldView);
}

//-----------------------Shadow Mapping Function--------------------------
void CalculateSplitDistances()
{
    float fIDM, fLog, fUniform;

    for (int i = 0; i < CDeferredRendering::CascadeCount; i++)
    {
        fIDM = i / (float)CDeferredRendering::CascadeCount;
        fLog = Scene->m_pRwCamera->nearPlane * (float)pow((CDeferredRendering::maxShadowDistance / Scene->m_pRwCamera->nearPlane), fIDM);
        fUniform = Scene->m_pRwCamera->nearPlane + (CDeferredRendering::maxShadowDistance - Scene->m_pRwCamera->nearPlane) * fIDM;
        m_faSplitDistances[i] = fLog * 0.3f + fUniform * (1.0f - 0.3f);
    }

  m_faSplitDistances[0] = 0.1f;
  m_faSplitDistances[CDeferredRendering::CascadeCount] = CDeferredRendering::maxShadowDistance;
	/*m_faSplitDistances[0] = -10.0f;
    m_faSplitDistances[1] = Scene->m_pRwCamera->nearPlane + 25.0f;
    m_faSplitDistances[2] = m_faSplitDistances[1] + (CDeferredRendering::maxShadowDistance - m_faSplitDistances[1]) * 0.3f;
    m_faSplitDistances[3] = m_faSplitDistances[1] + (CDeferredRendering::maxShadowDistance - m_faSplitDistances[1]) * 0.6f;
    m_faSplitDistances[4] = CDeferredRendering::maxShadowDistance;*/
}
void CDeferredRendering::ComputeShadowMap(IDirect3DSurface9*shadowSurface,
										  IDirect3DSurface9*shadowSurfaceC,
										  float distance,D3DXMATRIX*lightview,
										  D3DXMATRIX*lightproj,int cascadeNum)
{
	// 1) We need to stop current camera updating to avoid bugs.
	RwCameraEndUpdate(Scene->m_pRwCamera);
	RwCameraSetNearClipPlane(Scene->m_pRwCamera, m_faSplitDistances[cascadeNum-1]);
	RwCameraSetFarClipPlane(Scene->m_pRwCamera, m_faSplitDistances[cascadeNum]);
	Scene->m_pRwCamera->frameBuffer->width = ShadowMapSize;
	Scene->m_pRwCamera->frameBuffer->height = ShadowMapSize;
	RwCameraBeginUpdate(Scene->m_pRwCamera);
	// 2) We need to get some values(sun position and player position).
	RwV3D sunpos;
	CVector camPos;
	GetSunPosn(&sunpos,1);
	FindPlayerCoors(&camPos, 0);
//--------------Some compute code that i don't understand :)-------------
	m_vLightDirection.x = -sunpos.x;
	m_vLightDirection.y = -sunpos.y;
	m_vLightDirection.z = -sunpos.z;
	float fDistance;
	float faNear[3];
	float faFar[3];
	float faDiff[3]; // 0 = xaxis, 1 = yaxis, 2 = zaxis
	D3DXVECTOR3 vaAxis[3];
	D3DXVECTOR3 tmp = D3DXVECTOR3();
	D3DXVECTOR3 vCenterPosition = D3DXVECTOR3();
	int i, k;
	for (i = 0; i < 3; i++)
	{
		faNear[i] = FLT_MAX;
		faFar[i] = -FLT_MAX;
	}
	D3DXVec3Normalize(&vaAxis[2],&m_vLightDirection);
	D3DXVec3Cross(&tmp,m_vUpVector, &vaAxis[2]);
	D3DXVec3Normalize(&vaAxis[0],&tmp);
	D3DXVec3Cross(&tmp,&vaAxis[2], &vaAxis[0]);
	D3DXVec3Normalize(&vaAxis[1],&tmp);
	for (i = 0; i < 8; i++)
	{
		for (k = 0; k < 3; k++)
		{
			fDistance = D3DXVec3Dot(&vaAxis[k], 
				&D3DXVECTOR3( Scene->m_pRwCamera->frustumCorners[i].x,
							  Scene->m_pRwCamera->frustumCorners[i].y,
							  Scene->m_pRwCamera->frustumCorners[i].z));

			if (fDistance < faNear[k])
				faNear[k] = fDistance;

			if (fDistance > faFar[k])
				faFar[k] = fDistance;
		}
	}
	for (i = 0; i < 3; i++)
	{
		faDiff[i] = faFar[i] - faNear[i];
		vCenterPosition += vaAxis[i] * (faNear[i] + faFar[i]);
	}
	vCenterPosition *= 0.5f;

	float ZNear = 100.0f;
	float fLightZFar = faDiff[2] + ZNear;
	D3DXVECTOR3 vLightPosition = vCenterPosition - vaAxis[2] * (faDiff[2] * 0.5f + ZNear);
//----------------------------------------------------------------------
	IDirect3DSurface9* pOldRTSurf= NULL,*m_pZBuffer = NULL;

	D3DXMATRIX view,proj;
	// 3) We need to get our old transforms and surfaces
	g_Device->GetTransform(D3DTS_VIEW,&view);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	g_Device->GetRenderTarget(0, &pOldRTSurf);
	g_Device->GetDepthStencilSurface(&m_pZBuffer);
	pOldRTSurf->Release();
	m_pZBuffer->Release();
	// 4) We need to compute all needed matrix's.
	D3DXMatrixLookAtLH(lightview,&vLightPosition,
								 &vCenterPosition,
								 &D3DXVECTOR3(0,0,1));
	D3DXMatrixOrthoLH(lightproj,faDiff[0], faDiff[1], -ZNear, fLightZFar);
	//D3DXMatrixPerspectiveLH(lightproj,faDiff[0], faDiff[1], -ZNear, fLightZFar);
	// 5) We need to render scene from light position.
	g_Device->SetRenderTarget(0,shadowSurfaceC);
	g_Device->SetDepthStencilSurface(shadowSurface);
	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	CVehicleRender::m_pEffect->SetTechnique("Shadow");
	CObjectRender::m_pEffect->SetTechnique("Shadow");
	CPedsRender::m_pEffect->SetTechnique("Shadow");
	g_Device->SetTransform(D3DTS_VIEW,lightview);
	g_Device->SetTransform(D3DTS_PROJECTION,lightproj);
	RenderScene();
	RenderPedWeapons();
	// 6) We need to set rendering back.
	g_Device->SetRenderTarget(0,pOldRTSurf);
	g_Device->SetDepthStencilSurface(m_pZBuffer);
	SAFE_RELEASE(pOldRTSurf);
	SAFE_RELEASE(m_pZBuffer);
	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	g_Device->SetTransform(D3DTS_VIEW,&view);
	g_Device->SetTransform(D3DTS_PROJECTION,&proj);
}
//----------------------------------------------------------------------

//-----------------------------Idle function-----------------------------
void CDeferredRendering::Idle(void *a)
{
//----------------------------Values------------------------------------
	unsigned int time;
	RwV2d mousePos;
	D3DXVECTOR4 sun,cam;
//----------------------------------------------------------------------
//----------------------------Time Update-------------------------------
	do 
	time = GetTimeFromRenderStart();
	while(time / GetTimerDivider() - LastTickTime < 14);
	LastTickTime = GetTimeFromRenderStart() / GetTimerDivider();
	UpdateTimer();           // CTimer::Update
//----------------------------------------------------------------------
//----------------------------Init Functions-----------------------------
	InitSprite2dPerFrame();  // CSprite2d::InitPerFrame
	NumPointLights = 0;      // CPointLights::NumLights
	CLights::m_nNumLights = 0;
	InitFontPerFrame();      // CFont::InitPerFrame
	ProcessGame();           // CGame::Process
	ServiceDMAudio(DMAudio); // cDMAudio::Service
	SetLightsWithTimeOfDayColour(Scene->m_pRpWorld);
//----------------------------------------------------------------------
//----------------------------Main Loop---------------------------------
	if(!a)
		return;
	if(FrontEndMenuManager->m_bMenuActive || GetCameraScreenFadeStatus(TheCamera) == 2)
	{
		CalculateAspectRatio(); // CDraw::CalculateAspectRatio
		CameraSize(Scene->m_pRwCamera, 0, tan(gfFOV * 0.0087266462f), 1.0f);
		SetRenderWareCamera(Scene->m_pRwCamera); // CVisibilityPlugins::SetRenderWareCamera
		RwCameraClear(Scene->m_pRwCamera, gColourTop, 2);
		if(!RsCameraBeginUpdate(Scene->m_pRwCamera))
			return;
	}
	else
	{
		mousePos.x = RsGlobal->MaximumWidth * 0.5f;
		mousePos.y = RsGlobal->MaximumHeight * 0.5f;
		RsMouseSetPos(&mousePos);
		ConstructRendererRenderList(); // CRenderer::ConstructRenderList
		PreRenderRenderer();           // CRenderer::PreRender
		ProcessPedTasks();
		//CreateShadowManagerShadows(ShadowManager); // CShadowManager::CreateShadows
		if(LightningFlash) // CWeather::LightningFlash
		{
			Timecycle->m_nCurrentSkyBottomRed = 255;
			Timecycle->m_nCurrentSkyBottomGreen = 255;
			Timecycle->m_nCurrentSkyBottomBlue = 255;
			if(!DoRWStuffStartOfFrame_Horizon(255, 255, 255, 255, 255, 255, 255))
				return;
		}
		else if(!DoRWStuffStartOfFrame_Horizon(Timecycle->m_nCurrentSkyTopRed, Timecycle->m_nCurrentSkyTopGreen, Timecycle->m_nCurrentSkyTopBlue,
			Timecycle->m_nCurrentSkyBottomRed, Timecycle->m_nCurrentSkyBottomGreen, Timecycle->m_nCurrentSkyBottomBlue, 255))
			return;
		DefinedState();
		RwCameraSetFarClipPlane(Scene->m_pRwCamera, Timecycle->m_fCurrentFarClip);
		Scene->m_pRwCamera->fogPlane = Timecycle->m_fCurrentFogStart;
		//RenderMirrors();
		RwCameraEndUpdate(Scene->m_pRwCamera);
//---------------------------Initialization------------------------------
		CVehicleRender::m_pEffect->SetFloat("screenHeight",(float)RsGlobal->MaximumHeight);
		CVehicleRender::m_pEffect->SetFloat("screenWidth",(float)RsGlobal->MaximumWidth);
		CVehicleRender::m_pEffect->SetTexture("gtNoise",noise);
		CObjectRender::m_pEffect->SetFloat("screenHeight",(float)RsGlobal->MaximumHeight);
		CObjectRender::m_pEffect->SetFloat("screenWidth",(float)RsGlobal->MaximumWidth);
		CObjectRender::m_pEffect->SetTexture("gtNoise",noise);
		CPedsRender::m_pEffect->SetFloat("screenHeight",(float)RsGlobal->MaximumHeight);
		CPedsRender::m_pEffect->SetFloat("screenWidth",(float)RsGlobal->MaximumWidth);
		CPedsRender::m_pEffect->SetTexture("gtNoise",noise);
		CPostProcess::CreateRTs();
		for(int i =0;i<CascadeCount*2;i+=2){
			if(!shadow[i]){
				g_Device->CreateTexture(ShadowMapSize,ShadowMapSize,0,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&shadow[i],NULL);
				shadow[i]->GetSurfaceLevel(0,&shadowSurface[i]);
			}
		}
		for(int i =1;i<CascadeCount*2;i+=2){
			if(!shadow[i]){
				g_Device->CreateTexture(ShadowMapSize,ShadowMapSize,0,D3DUSAGE_DEPTHSTENCIL,D3DFMT_D24S8,D3DPOOL_DEFAULT,&shadow[i],NULL);
				shadow[i]->GetSurfaceLevel(0,&shadowSurface[i]);
			}
		}
		for(int i =0;i<3;i++) {
			if(!gbuffer[i]) {
				g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET, (i==2) ? D3DFMT_A32B32G32R32F : D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,&gbuffer[i],NULL);
				gbuffer[i]->GetSurfaceLevel(0,&gbSurface[i]);
			}
		}
		if(!lightingTexture) {
			g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&lightingTexture,NULL);
			lightingTexture->GetSurfaceLevel(0,&lightingSurface);
		}
		if(ppTCcount>0) {
			for(int i = 0; i<ppTCcount;i++) {
				if(!rtTmpSurface[i]) {
					g_Device->CreateTexture(surfWidth[i] == -1? RsGlobal->MaximumWidth:surfWidth[i],surfHeight[i] == -1? RsGlobal->MaximumHeight:surfHeight[i],0,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&rtTmpSurface[i],NULL);
					rtTmpSurface[i]->GetSurfaceLevel(0,&rsTmpSurface[i]);
				}
			}
		}
		RwCameraBeginUpdate(Scene->m_pRwCamera);
		IDirect3DSurface9* pOldRTSurf= NULL,*m_pZBuffer = NULL;
		D3DXMATRIX view,proj,viewproj,invview,invviewproj;
		RwV3D camPos;
		GetSunPosn((CVector*)&CGlobalValues::gm_SunPosition,1000);
		CalculateSplitDistances();
		FindPlayerCoors(&camPos, 0);
		//DrawWaterReflection();
		int oldWidth,oldHeight;
		oldWidth = Scene->m_pRwCamera->frameBuffer->width;
		oldHeight = Scene->m_pRwCamera->frameBuffer->height;
		for(int i = 0; i < CascadeCount; i++){
			ComputeShadowMap(shadowSurface[i+1],shadowSurface[i],150,&g_mLightView[i],&g_mLightProj[i],i+1);
		}
		RwCameraEndUpdate(Scene->m_pRwCamera);
		RwCameraSetNearClipPlane(Scene->m_pRwCamera, 0.1f);
		RwCameraSetFarClipPlane(Scene->m_pRwCamera, Timecycle->m_fCurrentFarClip);
		Scene->m_pRwCamera->frameBuffer->width = oldWidth;
		Scene->m_pRwCamera->frameBuffer->height = oldHeight;
		RwCameraBeginUpdate(Scene->m_pRwCamera);
		//RwCameraEndUpdate(Scene->m_pRwCamera);
		g_Device->GetTransform(D3DTS_VIEW,&view);
		g_Device->GetTransform(D3DTS_PROJECTION,&proj);
		D3DXMatrixInverse(&invview,NULL,&view);
		D3DXMatrixMultiply(&viewproj,&view,&proj);
		D3DXMatrixInverse(&invviewproj,NULL,&viewproj);
		cam = D3DXVECTOR4(camPos.x,camPos.y,camPos.z,1);
		CSkyRender::PreRender(&cam,&viewproj);
		D3DXCOLOR cc;
		cc.r = 0;cc.g = 0;cc.b = 0;cc.a = 1;
//----------------------------------------------------------------------
//-------------------Render into geometry buffer-------------------------
		g_Device->GetRenderTarget(0, &pOldRTSurf);
		pOldRTSurf->Release();
		g_Device->SetRenderTarget(0,gbSurface[0]);
		g_Device->SetRenderTarget(1,gbSurface[1]);
		g_Device->SetRenderTarget(2,gbSurface[2]);
		RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
		CObjectRender::m_pEffect->SetTechnique("Deferred");
		CVehicleRender::m_pEffect->SetTechnique("Deferred");
		CPedsRender::m_pEffect->SetTechnique("Deferred");
		D3DXCOLOR ambientColor,ambientColor2;
		ambientColor.r = (float)Timecycle->m_fCurrentAmbientRed;
		ambientColor.g = (float)Timecycle->m_fCurrentAmbientGreen;
		ambientColor.b = (float)Timecycle->m_fCurrentAmbientBlue;
		ambientColor.a = 1.0;
		m_pEffect->SetVector("gvAmbientColor", (D3DXVECTOR4 *)&ambientColor);
		ambientColor2.r = (float)Timecycle->m_fCurrentAmbientObjRed;
		ambientColor2.g = (float)Timecycle->m_fCurrentAmbientObjGreen;
		ambientColor2.b = (float)Timecycle->m_fCurrentAmbientObjBlue;
		ambientColor2.a = 1.0;
		m_pEffect->SetVector("gvAmbientColor2", (D3DXVECTOR4 *)&ambientColor2);
		m_pEffect->SetVector("fInverseViewportDimensions", &D3DXVECTOR4(1.0f/(float)RsGlobal->MaximumWidth,1.0f/(float)RsGlobal->MaximumHeight,1,1));
		m_pEffect->SetMatrix("gmViewProj",&viewproj);
		m_pEffect->SetMatrix("gmViewProjInv",&invviewproj);
		m_pEffect->SetMatrix("gmViewInv",&invview);
		sun = D3DXVECTOR4(camPos.x+(CGlobalValues::gm_SunPosition.x),camPos.y+(CGlobalValues::gm_SunPosition.y),camPos.z+(CGlobalValues::gm_SunPosition.z),1);
		
		RenderScene();
		RenderPedWeapons();
		CSkyRender::Render(&sun);
//----------------------------------------------------------------------
		//RwCameraEndUpdate(Scene->m_pRwCamera);
		//CSkyRender::Release();
		//RwCameraBeginUpdate(Scene->m_pRwCamera);
//-------------------Render Deferred Lighting----------------------------
		
		m_pEffect->SetTechnique("DefShad");
		D3DXMATRIX m_LightViewProj[4];
		g_Device->SetRenderTarget(0,lightingSurface);
		g_Device->SetRenderTarget(1,NULL);
		g_Device->SetRenderTarget(2,NULL);
		for(int i = 0; i < CascadeCount; i++) {
			D3DXMatrixMultiplyTranspose(&m_LightViewProj[i],&g_mLightView[i],&g_mLightProj[i]);
		}
		m_pEffect->SetMatrixArray("gmLightViewProj",m_LightViewProj,CascadeCount);
		// Set Textures
		m_pEffect->SetTexture("colorBuffer",gbuffer[0]);
		m_pEffect->SetTexture("normalSpecBuffer",gbuffer[1]);
		m_pEffect->SetTexture("shadowDepthBuffer",gbuffer[2]);
		CPostProcess::m_pEffect->SetTexture("tWPosDepth",gbuffer[2]);
		m_pEffect->SetTexture("noise",noise);
		CPostProcess::m_pEffect->SetTexture("tNoise",noise);
		m_pEffect->SetTexture("cubemap",cubemap);
		m_pEffect->SetTexture("test",(CascadeCount*2>=1) ? shadow[1] : NULL);
		m_pEffect->SetTexture("test2",(CascadeCount*2>=3) ? shadow[3] : NULL);
		m_pEffect->SetTexture("test3",(CascadeCount*2>=5) ? shadow[5] : NULL);
		m_pEffect->SetTexture("test4",(CascadeCount*2>=7) ? shadow[7] : NULL);
		m_pEffect->SetVector("SunColor",&D3DXVECTOR4(1-(*_daylightLightingState),1-(*_daylightLightingState),1-(*_daylightLightingState),1-(*_daylightLightingState)));
		m_pEffect->SetVector("ShadowParams",&D3DXVECTOR4((float)ShadowMapSize,1.0f/(float)ShadowMapSize,ShadowBias,1.0f));
		m_pEffect->SetVector("g_fSplitDistances",&D3DXVECTOR4(m_faSplitDistances[1],
															  m_faSplitDistances[2],
															  m_faSplitDistances[3],
															  m_faSplitDistances[4]));
		// ------------
		cam = *(D3DXVECTOR4*)GetCamPos();
		m_pEffect->SetVector("vDir",&D3DXVECTOR4(cam.x-camPos.x,cam.y-camPos.y,cam.z-camPos.z,1));
		m_pEffect->SetVector("lightDirection",&D3DXVECTOR4(CGlobalValues::gm_SunPosition.x+camPos.x,CGlobalValues::gm_SunPosition.y+camPos.y,CGlobalValues::gm_SunPosition.z+camPos.z,1));
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);
		DrawPostProcessPass(m_pEffect);
		m_pEffect->SetTechnique("DefShadPL");
		if(CLights::m_nNumLights > 0)
		{
			for(unsigned int l = 0; l < CLights::m_nNumLights; l++)
			{
				D3DXVECTOR3 pos;
				RECT sr;
				if(CLights::m_aLights[l].mode == LIGHT_MODE_POINT)
				{
					pos = D3DXVECTOR3(CLights::m_aLights[l].pos.x, CLights::m_aLights[l].pos.y, CLights::m_aLights[l].pos.z);
					sr = DetermineClipRect(pos, CLights::m_aLights[l].radius, view, proj, (float)RsGlobal->MaximumWidth, (float)RsGlobal->MaximumHeight);
					m_pEffect->SetVector("sLP",&D3DXVECTOR4(CLights::m_aLights[l].pos.x,CLights::m_aLights[l].pos.y,CLights::m_aLights[l].pos.z,1));
					m_pEffect->SetVector("PointLightColor",&D3DXVECTOR4(CLights::m_aLights[l].red,CLights::m_aLights[l].green,CLights::m_aLights[l].blue,1));
					m_pEffect->SetFloat("PointLightRange",CLights::m_aLights[l].radius);
					g_Device->SetScissorRect(&sr);
					RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
					RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
					DrawPostProcessPass(m_pEffect);
				}
			}
		}
//----------------------------------------------------------------------
//-----------Post-Processing and Alpha-Blended Stuff---------------------
		g_Device->SetRenderTarget(0,ppTCcount>0? rsTmpSurface[0]:pOldRTSurf);
		m_pEffect->SetTexture("lightBuffer",lightingTexture);
		m_pEffect->SetTechnique("DefShadL");
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDZERO);
		DrawPostProcessPass(m_pEffect);
		if(ppTCcount>0) {
			PostProcess(pOldRTSurf);
		}
		CParticleRender::m_pEffect->SetTexture("gtDepth",gbuffer[2]);
		CWaterRender::m_pEffect->SetTexture("gtDepth",gbuffer[2]);
		CWaterRender::m_pEffect->SetTexture("tScreen",gbuffer[0]);
		CWaterRender::m_pEffect->SetTexture("tRefl",cubemap);
		CWaterRender::m_pEffect->SetMatrix("gmRefl",&g_mReflViewProj);
		CImmediateRender::m_nCurrentRendering = IM_RENDER_WATER;
		RenderWater();
		CImmediateRender::m_nCurrentRendering = IM_RENDER_PARTICLES;
		RenderEffects();
		if((!TheCamera->m_BlurType || TheCamera->m_BlurType == 2) && TheCamera->m_ScreenReductionPercentage > 0.0 )
			SetCameraMotionBlurAlpha(TheCamera, 150); // CCamera::SetMotionBlurAlpha
		RenderCameraMotionBlur(TheCamera);            // CCamera::RenderMotionBlur
		CPostProcess::Render();
		rwD3D9RenderStateReset();
		Render2dStuff();
//----------------------------------------------------------------------
	}
//----------------------------------------------------------------------
	if(FrontEndMenuManager->m_bMenuActive)
		DrawMenuManagerFrontEnd(FrontEndMenuManager); // CMenuManager::DrawFrontEnd
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
//---------------------------Text---------------------------------------
	DoFade();
	DrawStyledText();
	DrawMessages(0); // CMessages::Display
	DrawFonts();     // CFont::DrawFonts
	if(CreditsOn)
	{
		if ( !FrontEndMenuManager->m_bMenuActive )
			RenderCredits();       // CCredits::Render
		if ( CreditsOn )
		{
			if ( !FrontEndMenuManager->m_bMenuActive )
				RenderCredits();   // CCredits::Render
		}
	}
	DebugDisplayTextBuffer(); // CDebug::DebugDisplayTextBuffer
	FlushObrsPrintfs();
//----------------------------------------------------------------------
	RwCameraEndUpdate(Scene->m_pRwCamera);
	RsCameraShowRaster(Scene->m_pRwCamera);
}
//----------------------------------------------------------------------

void CDeferredRendering::RenderScene()
{
	/*float camZ, nearClip;
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZTESTENABLE, FALSE);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, FALSE);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, FALSE);
	if(!gOcclReflectionsState)
	{
		//DoRWRenderHorizon();
		//RenderClouds(); // CClouds::Render
	}
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);*/
	//UpdateSunLightForCustomRenderingPipeline();
	RenderRoads();                      // CRenderer::RenderRoads
	//RenderCoronasReflections();         // CCoronas::RenderReflections
	RenderEverythingBarRoads();         // CRenderer::RenderEverythingBarRoads
	//RenderBrokenObjects(byte_BB4240, 0);
	RenderFadingInUnderwaterEntities(); // CRenderer::RenderFadingInUnderwaterEntities
	/*if(gCameraSeaDepth <= 0.0)
	{
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
		RenderWater(); //CWaterLevel::RenderWater
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLBACK);
	}*/
	RenderFadingInEntities(); // CRenderer::RenderFadingInEntities
	/*if(!gMirrorsRenderingState)
	{
		if(TheCamera->Cams[TheCamera->ActiveCam].Front.z <= 0.0)
			camZ = -TheCamera->Cams[TheCamera->ActiveCam].Front.z;
		else
			camZ = 0.0f;
		nearClip = ((flt_8CD4F0 * flt_8CD4EC * 0.25f - flt_8CD4F0 * flt_8CD4EC) * camZ + flt_8CD4F0 * flt_8CD4EC) * (Scene->m_pRwCamera->farPlane -
			Scene->m_pRwCamera->nearPlane);
		RwCameraEndUpdate(Scene->m_pRwCamera);
		RwCameraSetNearClipPlane(Scene->m_pRwCamera, nearClip + Scene->m_pRwCamera->nearPlane);
		RwCameraBeginUpdate(Scene->m_pRwCamera);
		sub_707F40();
		RenderStaticShadows(); // CShadows::RenderStaticShadows
		RenderStoredShadows(); // CShadows::RenderStoredShadows
		RwCameraEndUpdate(Scene->m_pRwCamera);
		RwCameraSetNearClipPlane(Scene->m_pRwCamera, Scene->m_pRwCamera->nearPlane);
		RwCameraBeginUpdate(Scene->m_pRwCamera);
	}*/
	//RenderBrokenObjects(byte_BB4240, 1);
	//RenderGrass(); // CGrass::Render
	/*RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
	if(!gOcclReflectionsState)
	{
		sub_7154B0();
		RenderRainStreaks();   // CWeather::RenderRainStreaks
		RenderSunReflection(); // CCoronas::RenderSunReflection
	}
	if(gCameraSeaDepth > 0.0)
	{
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
		RenderWater(); // CWaterLevel::RenderWater
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLBACK);
	}
	RenderStencil();*/
}

void __declspec(naked)CDeferredRendering::RenderParticlesType0()
{
	// Fucking particles!
	__asm mov eax, 0x53E193
	__asm call eax
	__asm retn;
}

void CDeferredRendering::RenderEffects()
{
	RenderBirds();        // CBirds::Render
	RenderSkidmarks();    // CSkidmarks::Render
	RenderRopes();        // CRopes::Render
	RenderGlass();        // CGlass::Render
	sub_733800();
	//RenderCoronas();      // CCoronas::Render
	RenderParticlesType0();
	RenderWaterCannons(); // CWaterCannons::Render
	sub_6E7760();
	RenderCloudMasked();
	RenderHighClouds();
	if(gNumCreatedHeliLights || gNumCreatedSearchlights)
	{
		SetRenderStatesForSpotLights();
		RenderHeliLights();   // CHeliLights::RenderAll
		RenderSearchlights(); // CSearchlights::RenderAll
		ResetRenderStatesForSpotLights();
	}
	RenderWeaponEffects(); // CWeaponEffects::Render
	if(gReplayMode != 1 && !GetPad(0)->field_10E) // CReplay::Mode  CPad::GetPad
		RenderWeaponTargetTriangle(FindPlayerPed(-1));
	RenderSpecialFX();          // CSpecialFX::Render
	RenderFogEffect();          // CPointLights::RenderFogEffect
	RenderFirstPersonVehicle(); // CRenderer::RenderFirstPersonVehicle
	//RenderPostProcess();
}