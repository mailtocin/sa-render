/********************************SA::Render source file*************************************/
/* File name   : CVehicleDrawable.cpp                                                      */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CVehicleDrawable controls vehicle drawing algorithm.                      */
/* File created: 11.05.2014                                                                */
/* File last ed: 13.05.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CVehicleDrawable.h"
#include "..\Headers\CGameIdle.h"
#include "..\Headers\CRenderTarget.h"
#include "..\Headers\CEffectMgr.h"
#include "..\Headers\CShadowMgr.h"
#include <game_sa\CTxdStore.h>

CEffect *CVehicleDrawable::m_pEffect;
RwTexture *CVehicleDrawable::m_pBurnoutTex;
RwTexture *CVehicleDrawable::m_pRaindropTex;

void CVehicleDrawable::Patch()
{
	CPatch::SetPointer(0x5D9FE4, RenderCallBack);
}

void CVehicleDrawable::Initialize()
{
	gGlobalsMgr.g_iResourceTXD = CTxdStore::AddTxdSlot("SARenderResources");
	CTxdStore::LoadTxd(gGlobalsMgr.g_iResourceTXD, "Render\\resources.txd");
	CTxdStore::AddRef(gGlobalsMgr.g_iResourceTXD);
	CTxdStore::PushCurrentTxd();
	CTxdStore::SetCurrentTxd(gGlobalsMgr.g_iResourceTXD);
	m_pBurnoutTex = RwReadTexture("burnout", "burnouta");
	m_pRaindropTex = RwReadTexture("raindrop", "raindropa");
	CTxdStore::PopCurrentTxd();
	m_pEffect = new CEffect(D3D_EFFECT_VEHICLE);
}

void CVehicleDrawable::RenderCallBack(RwResEntry *RepEntry, RpAtomic *Atomic, unsigned char Type, char Flags)
{
	bool bHasNoTexture, bHasAlpha, bBurnout;
	void* pIndexBuffer;
	RpGeometry *pGeometry;
	RpMaterial *pMaterial;
	D3DXMATRIX WorldViewProjection, World, mVP;
	RxD3D9InstanceData *pMesh;
	pGeometry = Atomic->geometry;
	rwD3D9EnableClippingIfNeeded(Atomic, Type);
	if (Flags & 8)
		bHasNoTexture = false;
	else
	{
		bHasNoTexture = true;
		rwD3D9SetTexture(NULL, NULL);
		rwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000u);
		rwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		rwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTOP_SELECTARG2);
		rwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		rwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTOP_SELECTARG2);
	}
	pIndexBuffer = RepEntry->header.indexBuffer;
	pMesh = &RepEntry->meshData;
	if (pIndexBuffer)
		rwD3D9SetIndices(pIndexBuffer);
	rwD3D9SetStreams(RepEntry->header.vertexStream, RepEntry->header.useOffsets);
	rwD3D9SetVertexDeclaration(RepEntry->header.vertexDeclaration);
	rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(Atomic->object.object.parent));

	// Matricles.
	GetWorldViewProj(&WorldViewProjection, RwFrameGetLTM(Atomic->object.object.parent), &mVP);
	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &WorldViewProjection, GT_WorldViewProj);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &WorldViewProjection,GT_WorldViewProj);

	rwD3D9VSGetWorldNormalizedTransposeMatrix(&World);
	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &World, GT_World);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &World, GT_World);

	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &gGlobalsMgr.g_mView, GT_View);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &gGlobalsMgr.g_mView, GT_View);

	// Vectors.
	m_pEffect->SetVector(m_pEffect->m_params.vehicle.LightPosition, &gGlobalsMgr.g_vSunPosition);
	m_pEffect->SetVector(m_pEffect->m_params.vehicle.ViewPosition, &gGlobalsMgr.g_vCameraPosition);
	D3DXVECTOR4 cAmbientColor0(Timecycle->m_fCurrentAmbientRed, Timecycle->m_fCurrentAmbientGreen, Timecycle->m_fCurrentAmbientBlue, 1.0f),
				cAmbientColor1(Timecycle->m_fCurrentAmbientObjRed, Timecycle->m_fCurrentAmbientObjGreen, Timecycle->m_fCurrentAmbientObjBlue, 1.0f);
	m_pEffect->SetVector(m_pEffect->m_params.vehicle.Ambient0, &cAmbientColor0);
	m_pEffect->SetVector(m_pEffect->m_params.vehicle.Ambient1, &cAmbientColor1);

	// Floats.
	m_pEffect->SetFloat(m_pEffect->m_params.vehicle.AmbientMultiplier, 0.3f);

	for (unsigned int i = 0; i < RepEntry->header.numMeshes; i++) {
		pMaterial = pMesh->material;
		bHasAlpha = pMesh->vertexAlpha || pMaterial->color.alpha != 255;

		//rwD3D9RenderStateVertexAlphaEnable(bHasAlpha);

		m_pEffect->SetColor(m_pEffect->m_params.vehicle.PaintColor, &pMaterial->color);
		if (pMaterial->m_pSpecular && (pMaterial->surfaceProps.m_dwFlags & 4))
			m_pEffect->SetFloat(m_pEffect->m_params.vehicle.Specularity, pMaterial->m_pSpecular->m_fLevel);
		else
			m_pEffect->SetFloat(m_pEffect->m_params.vehicle.Specularity, 0);
		if (pMaterial->m_pReflection && ((pMaterial->surfaceProps.m_dwFlags & 1) || (pMaterial->surfaceProps.m_dwFlags & 2)))
			m_pEffect->SetFloat(m_pEffect->m_params.vehicle.Reflectivity, RpMaterialGetFxEnvShininess(pMaterial));
		else
			m_pEffect->SetFloat(m_pEffect->m_params.vehicle.Reflectivity, 0);
		bBurnout = false;
		unsigned int ctest = *(DWORD*)&pMaterial->color & 0xFFFFFF;
		if (ctest <= 0xAF00FF){
			bBurnout = true;
			if (ctest != 0xAF00FF)
			{
				if (ctest > 0xFF3C)
				{
					if (ctest != 0xFFB9)
						bBurnout = false;
				}
				else
				{
					if (ctest != 0xFF3C && ctest != 0x3CFF && ctest != 0xAFFF)
						bBurnout = false;
				}
			}
		}
		if (ctest == 0xC8FF00 || ctest == 0xFF00FF || ctest == 0xFFFF00)
			bBurnout = true;
		if (CGameIdle::m_pReflectionRT[0] && CGameIdle::m_pReflectionRT[1]){
			m_pEffect->SetTexture("reflTex0", CGameIdle::m_pReflectionRT[0]->GetTex());
			m_pEffect->SetTexture("reflTex1", CGameIdle::m_pReflectionRT[1]->GetTex());
		}
		if (gRenderState != RENDERTYPE_SHADOW){
			if (!bBurnout) {
				if (pMaterial->texture) {
					if (gRenderState == RENDERTYPE_FORWARD)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.ForwardTextured);
					else if (gRenderState == RENDERTYPE_DEFERRED)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.DeferredTextured);
					m_pEffect->SetTexture(m_pEffect->m_params.vehicle.ColorTexture, pMaterial->texture);
					if (m_pRaindropTex)
						m_pEffect->SetTexture(m_pEffect->m_params.vehicle.RaindropTexture, m_pRaindropTex);
				}
				else{
					if (gRenderState == RENDERTYPE_FORWARD)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.ForwardPaint);
					else if (gRenderState == RENDERTYPE_DEFERRED)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.DeferredPaint);
				}
			}
			else
			{
				if (pMaterial->texture){
					if (gRenderState == RENDERTYPE_FORWARD)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.ForwardBurnoutTex);
					else if (gRenderState == RENDERTYPE_DEFERRED)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.DeferredBurnoutTex);

					m_pEffect->SetTexture(m_pEffect->m_params.vehicle.ColorTexture, pMaterial->texture);
				}
				else{
					if (gRenderState == RENDERTYPE_FORWARD)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.ForwardBurnout);
					else if (gRenderState == RENDERTYPE_DEFERRED)
						m_pEffect->SetTechnique(m_pEffect->m_techniques.vehicle.DeferredBurnoutTex);
				}
				if (m_pBurnoutTex)
					m_pEffect->SetTexture(m_pEffect->m_params.vehicle.BurnoutTexture, m_pBurnoutTex);
			}
			Render(&RepEntry->header, pMesh, m_pEffect);
		}
		else {
			CShadowMgr::m_pEffect->SetFloat(CShadowMgr::m_pEffect->m_params.shadows.Alpha, (float)pMaterial->color.alpha/255.0f);
			if (pMaterial->texture){
				CShadowMgr::m_pEffect->SetTexture(CShadowMgr::m_pEffect->m_params.shadows.ColorTexture, pMaterial->texture);
			}
			Render(&RepEntry->header, pMesh, CShadowMgr::m_pEffect);
		}
		pMesh++;
	}
	rwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	rwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	rwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	rwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
}