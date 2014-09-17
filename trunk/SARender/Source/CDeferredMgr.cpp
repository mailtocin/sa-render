/********************************SA::Render source file*************************************/
/* File name   : CDeferredMgr.cpp                                                          */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CDeferredMgr deferred rendering class.									   */
/* File created: 15.06.2014                                                                */
/* File last ed: 29.07.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CDeferredMgr.h"
#include "..\Headers\CRenderTarget.h"
#include "..\Headers\CGlobalsMgr.h"
#include "..\Headers\CEffectMgr.h"
#include "..\Headers\CLight.h"
#include "..\Headers\CShadowMgr.h"
#include "..\CGTAVTimeCycle.h"
CRenderTarget *CDeferredMgr::m_pGeomtryBuffer[3];
CRenderTarget *CDeferredMgr::m_pLightingBuffer;
CEffect *CDeferredMgr::m_pEffect;
ID3DXMesh *CDeferredMgr::PointLightMesh;

void CDeferredMgr::Initialize()
{
	m_pEffect = new CEffect(D3D_EFFECT_DEFERRED);
	m_pGeomtryBuffer[0] = new CRenderTarget(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight, 0, D3DFMT_A16B16G16R16F, false);
	m_pGeomtryBuffer[1] = new CRenderTarget(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight, 0, D3DFMT_A8R8G8B8, false);
	m_pGeomtryBuffer[2] = new CRenderTarget(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight, 0, D3DFMT_A8R8G8B8, false);
	m_pLightingBuffer = new CRenderTarget(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight, 0, D3DFMT_A16B16G16R16F, false);
	D3DXLoadMeshFromX("Render/Meshes/PointLight.x", D3DXMESH_SYSTEMMEM, g_Device, NULL, NULL, NULL, NULL, &PointLightMesh);
}
void CDeferredMgr::DrawLight(ID3DXMesh *pMesh, D3DXVECTOR4* vPos){
	D3DXMATRIX meshRotate, meshTranslate, meshScale, gm_World, gm_WorldViewProjection;
	D3DXMatrixRotationY(&meshRotate, D3DXToRadian(0));
	D3DXMatrixTranslation(&meshTranslate, vPos->x, vPos->y, vPos->z);
	D3DXMatrixMultiply(&gm_World, &meshRotate, &meshTranslate);
	D3DXMatrixMultiplyTranspose(&gm_WorldViewProjection, &gm_World, &gGlobalsMgr.g_mViewProj);
	m_pEffect->SetMatrix("mWorldViewProj", &gm_WorldViewProjection);
	m_pEffect->GetD3DEffect()->Begin(NULL, NULL);

	m_pEffect->GetD3DEffect()->BeginPass(0);
	g_Device->SetPixelShader(NULL);
	pMesh->DrawSubset(0);
	m_pEffect->GetD3DEffect()->EndPass();

	m_pEffect->GetD3DEffect()->BeginPass(1);
	pMesh->DrawSubset(0);
	m_pEffect->GetD3DEffect()->EndPass();

	m_pEffect->GetD3DEffect()->End();
}

void CDeferredMgr::RenderToGeometryBuffer(void* RenderFunc){
	RwCameraSetFarClipPlane(Scene->m_pRwCamera, CGTAVTimeCycle::GetCurrentFarClip());
	gRTMgr.GetRef();
	gRenderState = RENDERTYPE_DEFERRED;
	for (int i = 0; i < 3; i++)
		m_pGeomtryBuffer[i]->SetRenderTarget(i);
	g_Device->Clear(0, 0, 3, D3DXCOLOR((float)Timecycle->m_nCurrentSkyTopRed / 255.0f, (float)Timecycle->m_nCurrentSkyTopGreen / 255.0f, (float)Timecycle->m_nCurrentSkyTopBlue / 255.0f, 0.0), 1.0, 0);
	RenderRoads();
	RenderEverythingBarRoads();         // CRenderer::RenderEverythingBarRoads
	RenderFadingInUnderwaterEntities(); // CRenderer::RenderFadingInUnderwaterEntities
	RenderFadingInEntities();
	RenderPedWeapons();
}

void CDeferredMgr::RenderToScreenOutput(){
	gGlobalsMgr.ReInitViewProj();
	CGTAVTimeCycle::UptadteTime();
	// Textures
	m_pEffect->SetTexture("GeometryBuffer0", m_pGeomtryBuffer[0]->GetTex());
	m_pEffect->SetTexture("GeometryBuffer1", m_pGeomtryBuffer[1]->GetTex());
	m_pEffect->SetTexture("GeometryBuffer2", m_pGeomtryBuffer[2]->GetTex());
	m_pEffect->SetTexture("LightingBuffer",	m_pLightingBuffer->GetTex());

	// Vectors
	m_pEffect->SetVector("g_vLightPos", &gGlobalsMgr.g_vSunPosition);
	m_pEffect->SetVector("g_vViewPos", &gGlobalsMgr.g_vCameraPosition);
	D3DXVECTOR4 cAmbientColor0(Timecycle->m_fCurrentAmbientRed, Timecycle->m_fCurrentAmbientGreen, Timecycle->m_fCurrentAmbientBlue, 1.0f),
				cAmbientColor1(Timecycle->m_fCurrentAmbientObjRed, Timecycle->m_fCurrentAmbientObjGreen, Timecycle->m_fCurrentAmbientObjBlue, 1.0f);
	m_pEffect->SetVector(m_pEffect->m_globals.globals.Ambient0, &CGTAVTimeCycle::GetCurrentNatAmbLightColorUp());
	m_pEffect->SetVector(m_pEffect->m_globals.globals.Ambient1, &CGTAVTimeCycle::GetCurrentNatAmbLightColorDown());
	m_pEffect->SetVector("g_fInverseViewportDimensions", &D3DXVECTOR4(1.0f / (float)RsGlobal->MaximumWidth, 1.0f / (float)RsGlobal->MaximumHeight, 0, 0));

	// Floats.
	m_pEffect->SetFloat(m_pEffect->m_globals.globals.AmbientMultiplier, 0.5f);
	m_pEffect->SetFloat("g_fFarClip", CGTAVTimeCycle::GetCurrentFarClip());
	m_pEffect->SetFloat("g_fNearClip", Scene->m_pRwCamera->nearPlane);

	// Matricles
	m_pEffect->SetMatrix("gmView", &gGlobalsMgr.g_mView);
	m_pEffect->SetMatrix("gmInvViewProj", &gGlobalsMgr.g_mViewProjInv);

	m_pLightingBuffer->SetRenderTarget(0);
	m_pEffect->SetTechnique("StaticLight");
	DrawPostProcessPass(m_pEffect);
	m_pEffect->SetVector("vLightColor", &CGTAVTimeCycle::GetCurrentDirectLightColor());
	m_pEffect->SetTechnique("DirectLight");
	DrawPostProcessPass(m_pEffect);
	if (CLights::m_nNumLights > 0)
	{
		for (unsigned int l = 0; l < CLights::m_nNumLights; l++)
		{
			if (CLights::m_aLights[l].type == LIGHT_TYPE_OMNI)
			{
				m_pEffect->SetTechnique("PointLight");
				m_pEffect->SetVector("vLightPos", &D3DXVECTOR4(CLights::m_aLights[l].pos.x, CLights::m_aLights[l].pos.y, CLights::m_aLights[l].pos.z, 1));
				m_pEffect->SetVector("vLightColor", &D3DXVECTOR4(CLights::m_aLights[l].red, CLights::m_aLights[l].green, CLights::m_aLights[l].blue, 1));
				m_pEffect->SetFloat("fLightRange", CLights::m_aLights[l].radius);
				DrawLight(PointLightMesh, &D3DXVECTOR4(CLights::m_aLights[l].pos.x, CLights::m_aLights[l].pos.y, CLights::m_aLights[l].pos.z, CLights::m_aLights[l].radius));
			}
			if (CLights::m_aLights[l].type == LIGHT_TYPE_SPOT){
				m_pEffect->SetTechnique("SpotLight");
				m_pEffect->SetVector("vLightPos", &D3DXVECTOR4(CLights::m_aLights[l].pos.x, CLights::m_aLights[l].pos.y, CLights::m_aLights[l].pos.z, 1));
				m_pEffect->SetVector("vSpotLightDir", &D3DXVECTOR4(CLights::m_aLights[l].dir.x, CLights::m_aLights[l].dir.y, CLights::m_aLights[l].dir.z, 1));
				m_pEffect->SetVector("vLightColor", &D3DXVECTOR4(CLights::m_aLights[l].red, CLights::m_aLights[l].green, CLights::m_aLights[l].blue, 1));
				m_pEffect->SetFloat("fLightRange", CLights::m_aLights[l].radius);
				DrawLight(PointLightMesh, &D3DXVECTOR4(CLights::m_aLights[l].pos.x, CLights::m_aLights[l].pos.y, CLights::m_aLights[l].pos.z, CLights::m_aLights[l].radius));
			}
		}
	}
	gRTMgr.ResetRef();
	m_pEffect->SetTechnique("Combine");
	DrawPostProcessPass(m_pEffect);
}
void CDeferredMgr::ReInitialize()
{
	m_pGeomtryBuffer[0]->UpdateSize(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight);
	m_pGeomtryBuffer[1]->UpdateSize(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight);
	m_pGeomtryBuffer[2]->UpdateSize(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight);
	m_pLightingBuffer->UpdateSize(RsGlobal->MaximumWidth, RsGlobal->MaximumHeight);
}
void CDeferredMgr::ShutDown(){
	delete m_pEffect;
}