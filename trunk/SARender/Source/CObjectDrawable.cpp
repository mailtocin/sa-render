/********************************SA::Render source file*************************************/
/* File name   : CObjectDrawable.cpp                                                       */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CObjectDrawable controls object drawing algorithm.                        */
/* File created: 15.05.2014                                                                */
/* File last ed: 15.05.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CObjectDrawable.h"
#include "..\Headers\CGameIdle.h"
#include "..\Headers\CRenderTarget.h"
#include "..\Headers\CEffectMgr.h"
#include "..\Headers\CShadowMgr.h"
#include "..\CGTAVTimeCycle.h"

CEffect *CObjectDrawable::m_pEffect;

void CObjectDrawable::Patch()
{
	CPatch::SetPointer(0x75E55E, RenderCallBack);
	CPatch::SetPointer(0x7CB24B, RenderCallBack);
	CPatch::SetPointer(0x7578AE, RenderCallBack);
}

void CObjectDrawable::Initialize()
{
	m_pEffect = new CEffect(D3D_EFFECT_OBJECT);
}

void CObjectDrawable::RenderCallBack(RwResEntry *RepEntry, RpAtomic *Atomic, unsigned char Type, char Flags)
{
	bool bHasAlpha;
	void* pIndexBuffer;
	RpGeometry *pGeometry;
	RpMaterial *pMaterial;
	D3DXMATRIX WorldViewProjection, World, mVP;
	RxD3D9InstanceData *pMesh;
	pGeometry = Atomic->geometry;
	if (!(Flags & 8))
	{
		rwD3D9SetTexture(NULL, NULL);
	}
	pIndexBuffer = RepEntry->header.indexBuffer;
	pMesh = &RepEntry->meshData;
	if (pIndexBuffer)
		rwD3D9SetIndices(pIndexBuffer);
	rwD3D9SetStreams(RepEntry->header.vertexStream, RepEntry->header.useOffsets);
	rwD3D9SetVertexDeclaration(RepEntry->header.vertexDeclaration);

	rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(Atomic->object.object.parent));
	rwD3D9VSGetComposedTransformMatrix(&WorldViewProjection);
	GetWorldViewProj(&WorldViewProjection, RwFrameGetLTM(Atomic->object.object.parent), &mVP);
	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &WorldViewProjection, GT_WorldViewProj);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &WorldViewProjection, GT_WorldViewProj);

	rwD3D9VSGetWorldNormalizedTransposeMatrix(&World);
	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &World, GT_World);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &World, GT_World);

	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &gGlobalsMgr.g_mView, GT_View);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &gGlobalsMgr.g_mView, GT_View);

	m_pEffect->SetVector(m_pEffect->m_params.object.LightPosition, &gGlobalsMgr.g_vSunPosition);
	m_pEffect->SetVector(m_pEffect->m_params.object.ViewPosition, &gGlobalsMgr.g_vCameraPosition);
	m_pEffect->SetVector(m_pEffect->m_params.building.Ambient0, &CGTAVTimeCycle::GetCurrentNatAmbLightColorUp());
	m_pEffect->SetVector(m_pEffect->m_params.building.Ambient1, &CGTAVTimeCycle::GetCurrentNatAmbLightColorDown());
	m_pEffect->SetFloat(m_pEffect->m_params.object.AmbientMultiplier, 0.3f);
	m_pEffect->SetFloat(m_pEffect->m_params.object.ReflectDir, gGlobalsMgr.g_fReflDir);

	for (unsigned int i = 0; i < RepEntry->header.numMeshes; i++) {
		pMaterial = pMesh->material;
		bHasAlpha = pMesh->vertexAlpha || pMaterial->color.alpha != 255;
		if (pMaterial->texture&&!CGameIdle::m_bUseAlphaTestForTexAlpha)
			bHasAlpha = bHasAlpha || RwD3D9TextureHasAlpha((int)pMaterial->texture);

		m_pEffect->SetColor(m_pEffect->m_params.object.Color, &pMaterial->color);
		m_pEffect->SetBool("bEnableVertexBlend", pMesh->vertexAlpha>0);

		if (CGameIdle::m_pReflectionRT[0] && CGameIdle::m_pReflectionRT[1]){
			m_pEffect->SetTexture("reflTex0", CGameIdle::m_pReflectionRT[0]->GetTex());
			m_pEffect->SetTexture("reflTex1", CGameIdle::m_pReflectionRT[1]->GetTex());
		}
		if (pMaterial->m_pReflection && (pMaterial->surfaceProps.m_dwFlags & 1))
			m_pEffect->SetFloat(m_pEffect->m_params.object.Reflectivity, RpMaterialGetFxEnvShininess(pMaterial));
		else
			m_pEffect->SetFloat(m_pEffect->m_params.object.Reflectivity, 0);
		if (gRenderState != RENDERTYPE_SHADOW){
			if (pMaterial->texture) {
				if (gRenderState == RENDERTYPE_REFL)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.object.ReflectionTextured);
				else if (gRenderState == RENDERTYPE_FORWARD)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.object.ForwardTextured);
				else if (gRenderState == RENDERTYPE_DEFERRED)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.object.DeferredTextured);

				m_pEffect->SetTexture(m_pEffect->m_params.object.ColorTexture, pMaterial->texture);
			}
			else if (gRenderState != RENDERTYPE_REFL){
				if (gRenderState == RENDERTYPE_FORWARD)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.object.ForwardColor);
				else if (gRenderState == RENDERTYPE_DEFERRED)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.object.DeferredColor);
			}
			if (gRenderState == RENDERTYPE_FORWARD&&bHasAlpha){
				Render(&RepEntry->header, pMesh, m_pEffect);
			}
			if ((gRenderState == RENDERTYPE_DEFERRED&&!bHasAlpha)){
				Render(&RepEntry->header, pMesh, m_pEffect);
			}
		} else {
			CShadowMgr::m_pEffect->SetFloat(CShadowMgr::m_pEffect->m_params.shadows.Alpha, (float)pMaterial->color.alpha / 255.0f);
			if (pMaterial->texture){
				CShadowMgr::m_pEffect->SetTexture(CShadowMgr::m_pEffect->m_params.shadows.ColorTexture, pMaterial->texture);
			}
			Render(&RepEntry->header, pMesh, CShadowMgr::m_pEffect);
		}
		pMesh++;
	}
}