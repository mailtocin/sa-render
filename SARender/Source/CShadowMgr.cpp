#include "..\Headers\CShadowMgr.h"
#include "..\Headers\CGlobalsMgr.h"
#include "..\Headers\CVehicleDrawable.h"
#include "..\Headers\CBuildingDrawable.h"
#include "..\Headers\CObjectDrawable.h"
#include "..\Headers\CGUI.h"

CEffect *CShadowMgr::m_pEffect;
D3DXMATRIX CShadowMgr::g_mLightViewProj[2];
D3DXMATRIX CShadowMgr::g_mLightView[2];

CRenderTarget *CShadowMgr::m_pShadowRT[2];
CRenderTarget *CShadowMgr::m_pShadowDepthRT[2];

float CShadowMgr::m_fMaxDistance = 20;

float CShadowMgr::m_fMaxDistanceToRender;
int CShadowMgr::m_nShadowSize = 1024;
bool CShadowMgr::m_bRenderDayShadows = true;
bool CShadowMgr::m_bRenderNightShadows = false;
eSunShadowCenteredOn CShadowMgr::m_dwSunShadowCenterOn=SCO_FUSTRUM;

void CShadowMgr::Initialize()
{
	m_pEffect		 = new CEffect(D3D_EFFECT_SHADOWMAPPING);
	if (m_bRenderDayShadows){
		for (int i = 0; i < 2; i++)
		{
			m_pShadowRT[i] = new CRenderTarget(m_nShadowSize, m_nShadowSize, 0, D3DFMT_R16F, false);
			m_pShadowDepthRT[i] = new CRenderTarget(m_nShadowSize, m_nShadowSize, 0, D3DFMT_D24S8, true);
		}
		
	}
}

void CShadowMgr::CalculateMatrix(int iMatrixID)
{
	D3DXVECTOR3 vLightPos, vLightDir, vFrustrumCenter;
	vFrustrumCenter = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 vLightBasis[3];
	float faMinAABB[3];
	float faMaxAABB[3];
	float faLightDim[3];
	float fDistance;

	for (int i = 0; i < 3; i++)
	{
		faMinAABB[i] = FLT_MAX;
		faMaxAABB[i] = -FLT_MAX;
	}

	vLightBasis[2].x = gGlobalsMgr.g_vSunPosition.x;
	vLightBasis[2].y = gGlobalsMgr.g_vSunPosition.y;
	vLightBasis[2].z = gGlobalsMgr.g_vSunPosition.z;

	// Calculate light axis to transform our camera bbox to light dir.
	D3DXVec3Normalize(&vLightBasis[2], &vLightBasis[2]);
	D3DXVec3Cross(&vLightBasis[0], &D3DXVECTOR3(0, 0, 1), &vLightBasis[2]);
	D3DXVec3Normalize(&vLightBasis[0], &vLightBasis[0]);
	D3DXVec3Cross(&vLightBasis[1], &vLightBasis[0], &vLightBasis[2]);
	D3DXVec3Normalize(&vLightBasis[1], &vLightBasis[1]);
	
	// Calculate light-aligned camera bbox.
	for (int i = 0; i < 8; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			fDistance = D3DXVec3Dot(&vLightBasis[k], &D3DXVECTOR3(Scene->m_pRwCamera->frustumCorners[i].x, 
																  Scene->m_pRwCamera->frustumCorners[i].y, 
																  Scene->m_pRwCamera->frustumCorners[i].z));

			if (fDistance < faMinAABB[k])
				faMinAABB[k] = fDistance;

			if (fDistance > faMaxAABB[k])
				faMaxAABB[k] = fDistance;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		faLightDim[i] = faMaxAABB[i] - faMinAABB[i];
		vFrustrumCenter += vLightBasis[i] * (faMinAABB[i] + faMaxAABB[i]);
	}
	vFrustrumCenter *= 0.5;

	switch (m_dwSunShadowCenterOn)
	{
	case SCO_PLAYER:
		vFrustrumCenter.x = gGlobalsMgr.g_vPlayerPosition.x;
		vFrustrumCenter.y = gGlobalsMgr.g_vPlayerPosition.y;
		vFrustrumCenter.z = gGlobalsMgr.g_vPlayerPosition.z;
		break;
	case SCO_CAMERA:
		vFrustrumCenter.x = gGlobalsMgr.g_vCameraPosition.x;
		vFrustrumCenter.y = gGlobalsMgr.g_vCameraPosition.y;
		vFrustrumCenter.z = gGlobalsMgr.g_vCameraPosition.z;
		break;
	default:
		break;
	}
	
	float ZNear = 50.0f;
	float fLightZFar = faLightDim[2] + ZNear;
	vLightPos = vFrustrumCenter + vLightBasis[2] * (faLightDim[2] * 0.5f + ZNear);

	D3DXMATRIX mLightView, mLightProj;
	D3DXMatrixLookAtLH(&mLightView, &vLightPos, &vFrustrumCenter, &D3DXVECTOR3(0, 0, 1));

	D3DXMatrixOrthoLH(&mLightProj, faLightDim[0], faLightDim[1], -ZNear, fLightZFar*10);
	D3DXMatrixTranspose(&g_mLightView[iMatrixID], &mLightView);
	D3DXMatrixMultiplyTranspose(&g_mLightViewProj[iMatrixID], &mLightView, &mLightProj);

	m_fMaxDistanceToRender = fLightZFar;

	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);

	gGlobalsMgr.ReInitViewProj(&mLightView, &mLightProj);
}
void CShadowMgr::SetShadowParamsToShader(CEffect *pEffect)
{
	pEffect->SetTexture("ShadowBuffer0", m_pShadowRT[0]->GetTex());
	pEffect->SetTexture("ShadowBuffer1", m_pShadowRT[1]->GetTex());
	pEffect->SetMatrixArray("mShadowMatrix", g_mLightViewProj,2);
	pEffect->SetMatrixArray("mLightViewMatrix", g_mLightView, 2);
	pEffect->SetVector("g_vShadowParams", &D3DXVECTOR4((float)m_nShadowSize, 1.0f / (float)m_nShadowSize, 0.0f, 0.0));
	pEffect->SetFloat("g_fMaxShadowDistance", m_fMaxDistanceToRender);
}
void CShadowMgr::Render(eShadowRenderType RenderType, float fDistance, int iMatrixID){
	
	RwCameraEndUpdate(Scene->m_pRwCamera);
	RwCameraSetNearClipPlane(Scene->m_pRwCamera, 0.1f);
	RwCameraSetFarClipPlane(Scene->m_pRwCamera, fDistance);
	Scene->m_pRwCamera->frameBuffer->width = m_nShadowSize;
	Scene->m_pRwCamera->frameBuffer->height = m_nShadowSize;
	RwCameraBeginUpdate(Scene->m_pRwCamera);

	gGlobalsMgr.GetViewProjRef();
	gRTMgr.GetRef();
	gRTMgr.GetZRef();

	m_pShadowRT[iMatrixID]->SetRenderTarget(0);
	m_pShadowDepthRT[iMatrixID]->SetRenderTarget(0);

	gRenderState = RENDERTYPE_SHADOW;

	CalculateMatrix(iMatrixID);

	switch (RenderType)
	{
	case SRT_DYNAMIC:
		RenderFadingInEntities();
		RenderFadingInUnderwaterEntities();
		RenderPedWeapons();
		break;
	case SRT_STATIC:
		RenderEverythingBarRoads();
		RenderRoads();
		break;
	case SRT_BOTH:
		RenderEverythingBarRoads();
		RenderRoads();
		RenderFadingInEntities();
		RenderPedWeapons();
		break;
	default:
		break;
	}

	gRTMgr.ResetRef();
	gRTMgr.ResetZRef();
	gGlobalsMgr.ResetViewProjRef();
}

void CShadowMgr::Update() {
	Render(SRT_BOTH, m_fMaxDistance,0);
	Render(SRT_BOTH, max(m_fMaxDistance*m_fMaxDistance, Timecycle->m_fCurrentFarClip), 1);
	RwCameraEndUpdate(Scene->m_pRwCamera);
	Scene->m_pRwCamera->frameBuffer->width = RsGlobal->MaximumWidth;
	Scene->m_pRwCamera->frameBuffer->height = RsGlobal->MaximumHeight;
	RwCameraBeginUpdate(Scene->m_pRwCamera);
}
void CShadowMgr::Shutdown() {
	delete m_pEffect;
}