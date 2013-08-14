#include "CDeferredRendering.h"
#include "CPatch.h"
#include "CDebug.h"
#include "CVehicleRender.h"
#include "CObjectRender.h"
#include "CPedsRender.h"
#include "CLights.h"
#include "CSkyRender.h"
#include <stdio.h>
#include <sstream>

// Shadows textures/surfaces
IDirect3DTexture9 *CDeferredRendering::shadow[4];
IDirect3DSurface9 *CDeferredRendering::shadowSurface[4];
// Geometry Buffer textures/surfaces
IDirect3DTexture9 *CDeferredRendering::gbuffer[3];
IDirect3DSurface9 *CDeferredRendering::gbSurface[3];
// Temporary Post-Processing textures/surfaces
IDirect3DTexture9 **CDeferredRendering::rtTmpSurface;
IDirect3DSurface9 **CDeferredRendering::rsTmpSurface;
// Lighting Buffer textures/surfaces
IDirect3DTexture9 *CDeferredRendering::lightingTexture;
IDirect3DSurface9 *CDeferredRendering::lightingSurface;
// Day/Night state
float* _daylightLightingState = (float*)0x8D12C0;
// Post-Process textures/surfaces count
int CDeferredRendering::ppTCcount;
// Post-Process textures/surfaces width/height
int *CDeferredRendering::surfWidth;
int *CDeferredRendering::surfHeight;
// Shadow View/Projection Matrix
D3DXMATRIX CDeferredRendering::g_mLightView[2];
D3DXMATRIX CDeferredRendering::g_mLightProj;
D3DXMATRIX CDeferredRendering::g_mLightProj2;
// Deferred/Post-Process Shader
ID3DXEffect *CDeferredRendering::m_pEffect;
// Noise texture for Post-Process
IDirect3DTexture9 *noise;
// Cubemap for Reflections
IDirect3DCubeTexture9 *CDeferredRendering::cubemap;
// D3D parameters
D3DPRESENT_PARAMETERS *g_D3Dpp = (D3DPRESENT_PARAMETERS *) 0xC9C040;
// Shadow stuff
float m_fExtraDistance;
D3DXVECTOR3 *m_vUpVector;
D3DXVECTOR3 m_vLightDirection;

// Setup function
bool CDeferredRendering::Setup()
{
	g_D3Dpp->MultiSampleQuality = 0;
	g_D3Dpp->MultiSampleType = D3DMULTISAMPLE_NONE;
	ID3DXBuffer *errors;
	HRESULT result;
	m_vUpVector = new D3DXVECTOR3(0, 1, 0);
	// Loading shader... TODO: Load it from folder...
	result = D3DXCreateEffectFromFile(g_Device,"deferred.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CDeferredRendering", "deferred", result))	{
		return false;
	}

//-------------------------Loading textures-------------------------------
	D3DXCreateTextureFromFile(g_Device,"noise.png",&noise);
	D3DXCreateCubeTextureFromFile(g_Device,"grace_diffuse_cube.dds",&cubemap);
//------------------------------------------------------------------------

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

//--------------------Full-Screen Quad Drawing Function-------------------
void DrawFullScreenQuad() {
	IDirect3DVertexDeclaration9*  VertDecl = NULL;
	struct Vertex {
		D3DXVECTOR2 pos;
		D3DXVECTOR2 tex_coord;
	}quad[4];
	// 1) We need to create quad verticles and texture coordinates.
	quad[0].pos = D3DXVECTOR2(-1,-1); quad[0].tex_coord = D3DXVECTOR2(0,1);
	quad[1].pos = D3DXVECTOR2(-1,1);  quad[1].tex_coord = D3DXVECTOR2(0,0);
	quad[2].pos = D3DXVECTOR2(1,-1);  quad[2].tex_coord = D3DXVECTOR2(1,1);
	quad[3].pos = D3DXVECTOR2(1,1);   quad[3].tex_coord = D3DXVECTOR2(1,0);

	// 2) We need to create quad vertex declaration.
	const D3DVERTEXELEMENT9 Decl[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	g_Device->CreateVertexDeclaration( Decl, &VertDecl );
	// 3) Finnaly we need to set it all and draw it.
	g_Device->SetVertexDeclaration(VertDecl);
	g_Device->SetRenderState(D3DRS_CULLMODE,rwCULLMODECULLNONE);
	g_Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex));
	// 4) And don't forget to release it, otherwise you can crash.
	SAFE_RELEASE(VertDecl);
}
//------------------------------------------------------------------------

// Post-Process Drawing Pass.... TODO: idk what to do with it... maybe make passes count in value?
void CDeferredRendering::DrawPostProcessPass() {
	UINT pPasses;				  // Pass count - unused but we can set it to use
	m_pEffect->Begin(&pPasses,0); // This function begin effect using and outputs pass count.
	m_pEffect->BeginPass(0);	  // This function begins pass(here we use only first pass for now).
	DrawFullScreenQuad();		  // Call Full-Screen Quad Drawing Function to draw Full-Screen Quad!
	m_pEffect->EndPass();		  // This function ends current pass. Don't forget it!
	m_pEffect->End();			  // This function ends effect using. Don't forget it!
}

//--------------------------On Lost Device--------------------------------
void CDeferredRendering::Lost()
{
	for(int i = 0;i<4;i++) {
		SAFE_RELEASE(shadowSurface[i]);
		SAFE_RELEASE(shadow[i]);
	}
	for(int i = 0;i<3;i++) {
		SAFE_RELEASE(gbSurface[i]);
		SAFE_RELEASE(gbuffer[i]);
	}
	SAFE_RELEASE(lightingSurface);
	SAFE_RELEASE(lightingTexture);
	for(int i = 0; i<ppTCcount;i++){
		SAFE_RELEASE(rsTmpSurface[i]);
		SAFE_RELEASE(rtTmpSurface[i]);
	}
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}
//------------------------------------------------------------------------

//---------------------------Patch Function-------------------------------
void CDeferredRendering::Patch()
{
	CPatch::RedirectCall(0x53ECBD, Idle);
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
		DrawPostProcessPass();
	}
	g_Device->SetRenderTarget(0,outSurf);
	tempString = "PostProcess_";
	tempString += std::to_string((long double)ppTCcount - 1);
	m_pEffect->SetTechnique((char*)tempString.c_str());

	tempString = "screenBuffer_";
	tempString += std::to_string((long double)ppTCcount - 1);
	m_pEffect->SetTexture((char*)tempString.c_str(),rtTmpSurface[ppTCcount-1]);
	DrawPostProcessPass();
}
//------------------------------------------------------------------------

// Cubemap rendering.. TODO: Make it WORK!!!!1111
void CDeferredRendering::DrawCubemap(){
	if(!cubemap){
		g_Device->CreateCubeTexture(64,0,D3DUSAGE_RENDERTARGET,
									D3DFMT_R8G8B8,D3DPOOL_DEFAULT,
									&cubemap,NULL);
	}
	IDirect3DSurface9* pOldRTSurf= NULL,*m_pZBuffer = NULL;
	D3DXMATRIX view,proj;
	g_Device->GetTransform(D3DTS_VIEW,&view);
	g_Device->GetTransform(D3DTS_PROJECTION, &proj);
	g_Device->GetRenderTarget(0, &pOldRTSurf);
	g_Device->GetDepthStencilSurface(&m_pZBuffer);
	pOldRTSurf->Release();
	m_pZBuffer->Release();
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/2, 1.0f, 0.5f, 1000.0f);
	CVector camPos;
	FindPlayerCoors(&camPos, 0);
	for(DWORD i=0; i<6; i++)
	{
		// Standard view that will be overridden below
		D3DXVECTOR3 vEnvEyePt = D3DXVECTOR3(camPos.x, camPos.y, camPos.z);
		D3DXVECTOR3 vLookatPt, vUpVec;

		switch(i)
		{
		case D3DCUBEMAP_FACE_POSITIVE_X:
			vLookatPt = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			vLookatPt = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			vLookatPt = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			vLookatPt = D3DXVECTOR3(0.0f,-1.0f, 0.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
			vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			vLookatPt = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		}
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView, &vEnvEyePt, &(vLookatPt+vEnvEyePt), &vUpVec);
		IDirect3DSurface9 *pFace;
		cubemap->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pFace);
		g_Device->SetRenderTarget(0,pFace);
		SAFE_RELEASE(pFace);
		g_Device->SetDepthStencilSurface(m_pZBuffer);
		RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
		g_Device->SetTransform(D3DTS_VIEW,&matView);
		g_Device->SetTransform(D3DTS_PROJECTION,&matProj);
		CObjectRender::m_pEffect->SetTechnique("Shadow");
		CVehicleRender::m_pEffect->SetTechnique("Shadow");
		CPedsRender::m_pEffect->SetTechnique("Shadow");
		RenderScene();
		RenderPedWeapons();
	}
	g_Device->SetRenderTarget(0,pOldRTSurf);
	g_Device->SetDepthStencilSurface(m_pZBuffer);
	SAFE_RELEASE(pOldRTSurf);
	SAFE_RELEASE(m_pZBuffer);
	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	g_Device->SetTransform(D3DTS_VIEW,&view);
	g_Device->SetTransform(D3DTS_PROJECTION,&proj);
}

//-----------------------Shadow Mapping Function--------------------------
void CDeferredRendering::ComputeShadowMap(IDirect3DSurface9*shadowSurface,
										  IDirect3DSurface9*shadowSurfaceC,
										  float distance,D3DXMATRIX*lightview,
										  D3DXMATRIX*lightproj,float dd)
{
	// 1) We need to stop current camera updating to avoid bugs.
	//RwCameraEndUpdate(Scene->m_pRwCamera);
	// 2) We need to get some values(sun position and player position).
	RwV3D sunpos;
	CVector camPos;
	GetSunPosn(&sunpos,distance);
	FindPlayerCoors(&camPos, 0);
//--------------Some compute code that i don't understand :)-------------
	m_vLightDirection.x = sunpos.x;
	m_vLightDirection.y = sunpos.y;
	m_vLightDirection.z = sunpos.z;
	float fDistance;
	float faNear[3];
	float faFar[3];
	float faDiff[3]; // 0 = xaxis, 1 = yaxis, 2 = zaxis
	D3DXVECTOR3 vaAxis[3];
	D3DXVECTOR3 tmp;
	int i, k;
	for (i = 0; i < 3; i++)
	{
		faNear[i] = FLT_MAX;
		faFar[i] = FLT_MIN;
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
	}
	float ZNear = 50.0f;
	float fLightZFar = faDiff[2] + ZNear;
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
	D3DXMatrixLookAtLH(lightview,&D3DXVECTOR3(camPos.x+(sunpos.x),
											  camPos.y+(sunpos.y),
											  camPos.z+(sunpos.z)),
								 &D3DXVECTOR3(camPos.x,camPos.y,camPos.z),
								 &D3DXVECTOR3(0,1,0));
	D3DXMatrixOrthoLH(lightproj,faDiff[0]/dd, faDiff[1]/dd, -ZNear, fLightZFar);
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
		CameraSize(Scene->m_pRwCamera, 0, tan(gfFOV * 0.0087266462f), gfAspectRatio);
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
		CreateShadowManagerShadows(ShadowManager); // CShadowManager::CreateShadows
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
		//RwCameraEndUpdate(Scene->m_pRwCamera);
//---------------------------Initialization------------------------------
		CVehicleRender::m_pEffect->SetFloat("screenHeight",(float)RsGlobal->MaximumHeight);
		CVehicleRender::m_pEffect->SetFloat("screenWidth",(float)RsGlobal->MaximumWidth);
		CVehicleRender::m_pEffect->SetTexture("noise",noise);
		CObjectRender::m_pEffect->SetFloat("screenHeight",(float)RsGlobal->MaximumHeight);
		CObjectRender::m_pEffect->SetFloat("screenWidth",(float)RsGlobal->MaximumWidth);
		CPedsRender::m_pEffect->SetFloat("screenHeight",(float)RsGlobal->MaximumHeight);
		CPedsRender::m_pEffect->SetFloat("screenWidth",(float)RsGlobal->MaximumWidth);
		
		for(int i =0;i<4;i+=2){
			if(!shadow[i]){
				g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&shadow[i],NULL);
				shadow[i]->GetSurfaceLevel(0,&shadowSurface[i]);
			}
		}
		for(int i =1;i<4;i+=2){
			if(!shadow[i]){
				g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_DEPTHSTENCIL,D3DFMT_D24S8,D3DPOOL_DEFAULT,&shadow[i],NULL);
				shadow[i]->GetSurfaceLevel(0,&shadowSurface[i]);
			}
		}
		
		for(int i =0;i<3;i++) {
			if(!gbuffer[i]) {
				g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,i==2? D3DFMT_A32B32G32R32F:D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT,&gbuffer[i],NULL);
				gbuffer[i]->GetSurfaceLevel(0,&gbSurface[i]);
			}
		}
		if(!lightingTexture) {
			g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,D3DFMT_A16B16G16R16F,D3DPOOL_DEFAULT,&lightingTexture,NULL);
			lightingTexture->GetSurfaceLevel(0,&lightingSurface);
		}
		if(ppTCcount>1) {
			for(int i = 0; i<ppTCcount;i++) {
				if(!rtTmpSurface[i]) {
					g_Device->CreateTexture(surfWidth[i] == -1? RsGlobal->MaximumWidth:surfWidth[i],surfHeight[i] == -1? RsGlobal->MaximumHeight:surfHeight[i],0,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&rtTmpSurface[i],NULL);
					rtTmpSurface[i]->GetSurfaceLevel(0,&rsTmpSurface[i]);
				}
			}
		}
		IDirect3DSurface9* pOldRTSurf= NULL,*m_pZBuffer = NULL;
		D3DXMATRIX view,proj,viewproj,invview,invviewproj;
		RwV3D camPos;
		GetSunPosn((CVector*)&CGlobalValues::gm_SunPosition,1000);

		FindPlayerCoors(&camPos, 0);
		//RwCameraBeginUpdate(Scene->m_pRwCamera);
		ComputeShadowMap(shadowSurface[1],shadowSurface[0],150,&g_mLightView[0],&g_mLightProj,45);
		ComputeShadowMap(shadowSurface[3],shadowSurface[2],150,&g_mLightView[1],&g_mLightProj2,5);
		//DrawCubemap();
		//RwCameraEndUpdate(Scene->m_pRwCamera);
		g_Device->GetTransform(D3DTS_VIEW,&view);
		g_Device->GetTransform(D3DTS_PROJECTION,&proj);
		D3DXMatrixInverse(&invview,NULL,&view);
		D3DXMatrixMultiply(&viewproj,&view,&proj);
		D3DXMatrixInverse(&invviewproj,NULL,&viewproj);
		cam = D3DXVECTOR4(camPos.x,camPos.y,camPos.z,1);
		CSkyRender::PreRender(&cam,&viewproj);
		D3DXCOLOR cc;
		cc.r = 1;cc.g = 1;cc.b = 1;cc.a = 1;

		//RwCameraBeginUpdate(Scene->m_pRwCamera);
//----------------------------------------------------------------------
//-------------------Render into geometry buffer-------------------------
		g_Device->GetRenderTarget(0, &pOldRTSurf);
		pOldRTSurf->Release();
		g_Device->SetRenderTarget(0,gbSurface[0]);
		g_Device->SetRenderTarget(1,gbSurface[1]);
		g_Device->SetRenderTarget(2,gbSurface[2]);
		//g_Device->Clear(0,NULL,D3DCLEAR_STENCIL||D3DCLEAR_TARGET||D3DCLEAR_ZBUFFER,cc,0,0);
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
		m_pEffect->SetMatrix("gmViewProj",&viewproj);
		m_pEffect->SetMatrix("gmViewProjInv",&invviewproj);
		m_pEffect->SetMatrix("gmViewInv",&invview);
		sun = D3DXVECTOR4(camPos.x+(CGlobalValues::gm_SunPosition.x),camPos.y+(CGlobalValues::gm_SunPosition.y),camPos.z+(CGlobalValues::gm_SunPosition.z),1);
		CSkyRender::Render(&sun);
		RenderScene();
		RenderPedWeapons();
//----------------------------------------------------------------------
		CSkyRender::Release();
		DWORD dwOldFVF;
		DWORD oDB,oSB,oBO,oAB,oAT;
		const DWORD dwFVF_POST = D3DFVF_XYZRHW | D3DFVF_TEX1;
//-------------------Render Deferred Lighting----------------------------
		g_Device->GetFVF(&dwOldFVF);
		GetCurrentStates(&oDB,&oSB,&oBO,&oAB,&oAT);
		g_Device->SetFVF(dwFVF_POST);
		m_pEffect->SetTechnique("DefShad");
		D3DXMATRIX m_LightViewProj[2];
		g_Device->SetRenderTarget(0,lightingSurface);
		//g_Device->SetRenderTarget(0,pOldRTSurf);
		g_Device->SetRenderTarget(1,NULL);
		g_Device->SetRenderTarget(2,NULL);
		D3DXMatrixMultiplyTranspose(&m_LightViewProj[0],&g_mLightView[0],&g_mLightProj);
		D3DXMatrixMultiplyTranspose(&m_LightViewProj[1],&g_mLightView[1],&g_mLightProj2);
		m_pEffect->SetMatrixArray("gmLightViewProj",m_LightViewProj,2);
		// Set Textures
		m_pEffect->SetTexture("colorBuffer",gbuffer[0]);
		m_pEffect->SetTexture("normalSpecBuffer",gbuffer[1]);
		m_pEffect->SetTexture("shadowDepthBuffer",gbuffer[2]);
		m_pEffect->SetTexture("noise",noise);
		m_pEffect->SetTexture("cubemap",cubemap);
		m_pEffect->SetTexture("test",shadow[1]);
		m_pEffect->SetTexture("test2",shadow[3]);
		// ------------
		cam = *(D3DXVECTOR4*)GetCamPos();
		m_pEffect->SetVector("vDir",&D3DXVECTOR4(cam.x-camPos.x,cam.y-camPos.y,cam.z-camPos.z,1));
		m_pEffect->SetVector("lightDirection",&D3DXVECTOR4(CGlobalValues::gm_SunPosition.x+camPos.x,CGlobalValues::gm_SunPosition.y+camPos.y,CGlobalValues::gm_SunPosition.z+camPos.z,1));
		DrawPostProcessPass();
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
					sr = DetermineClipRect(pos, CLights::m_aLights[l].radius, view, proj, 1024, 768);
					m_pEffect->SetVector("sLP",&D3DXVECTOR4(CLights::m_aLights[l].pos.x,CLights::m_aLights[l].pos.y,CLights::m_aLights[l].pos.z,1));
					m_pEffect->SetVector("PointLightColor",&D3DXVECTOR4(CLights::m_aLights[l].red,CLights::m_aLights[l].green,CLights::m_aLights[l].blue,1));
					m_pEffect->SetFloat("PointLightRange",CLights::m_aLights[l].radius);
					g_Device->SetScissorRect(&sr);
					DrawPostProcessPass();
				}
			}
		}
//----------------------------------------------------------------------
//-----------Post-Processing and Alpha-Blended Stuff---------------------
		g_Device->SetRenderTarget(0,rsTmpSurface[0]);
		m_pEffect->SetTexture("lightBuffer",lightingTexture);
		m_pEffect->SetTechnique("DefShadL");
		DrawPostProcessPass();
		PostProcess(pOldRTSurf);
		g_Device->SetFVF(dwOldFVF);
		SetOldStates(oDB,oSB,oBO,oAB,oAT);
		//RenderWater();
		//RenderClouds();
		RenderEffects();
		//sub_53E8D0(g_Unk);
		if((!TheCamera->m_BlurType || TheCamera->m_BlurType == 2) && TheCamera->m_ScreenReductionPercentage > 0.0 )
			SetCameraMotionBlurAlpha(TheCamera, 150); // CCamera::SetMotionBlurAlpha
		RenderCameraMotionBlur(TheCamera);            // CCamera::RenderMotionBlur
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
	UpdateSunLightForCustomRenderingPipeline();
	RenderRoads();                      // CRenderer::RenderRoads
	//RenderCoronasReflections();         // CCoronas::RenderReflections
	RenderEverythingBarRoads();         // CRenderer::RenderEverythingBarRoads
	RenderBrokenObjects(byte_BB4240, 0);
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
	RenderBrokenObjects(byte_BB4240, 1);
	/*RenderGrass(); // CGrass::Render
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
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