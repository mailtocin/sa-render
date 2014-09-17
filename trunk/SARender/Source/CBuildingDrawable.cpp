/********************************SA::Render source file*************************************/
/* File name   : CBuildingDrawable.cpp                                                     */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CBuildingDrawable controls building drawing algorithm.                    */
/* File created: 12.05.2014                                                                */
/* File last ed: 12.05.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CBuildingDrawable.h"
#include "..\Headers\CGameIdle.h"
#include "..\Headers\CRenderTarget.h"
#include "..\Headers\CEffectMgr.h"
#include "..\Headers\CShadowMgr.h"

CEffect *CBuildingDrawable::m_pEffect;

void CBuildingDrawable::Patch()
{
	CPatch::SetPointer(0x7578AE, RenderCallBack);
	CPatch::SetPointer(0x5D67F4, RenderCallBack);
}

void CBuildingDrawable::Initialize()
{
	m_pEffect = new CEffect(D3D_EFFECT_BUILDING);
}

void CBuildingDrawable::RenderCallBack(RwResEntry *RepEntry, RpAtomic *Atomic, unsigned char Type, char Flags)
{
	bool bHasNoTexture, bHasAlpha;
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
	GetWorldViewProj(&WorldViewProjection, RwFrameGetLTM(Atomic->object.object.parent), &mVP);
	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &WorldViewProjection, GT_WorldViewProj);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &WorldViewProjection, GT_WorldViewProj);

	rwD3D9VSGetWorldNormalizedTransposeMatrix(&World);
	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &World, GT_World);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &World, GT_World);

	gGlobalsMgr.SetGlobalMatrixToEffect(m_pEffect, &gGlobalsMgr.g_mView, GT_View);
	gGlobalsMgr.SetGlobalMatrixToEffect(CShadowMgr::m_pEffect, &gGlobalsMgr.g_mView, GT_View);

	m_pEffect->SetVector(m_pEffect->m_params.building.LightPosition, &gGlobalsMgr.g_vSunPosition);
	m_pEffect->SetVector(m_pEffect->m_params.building.ViewPosition, &gGlobalsMgr.g_vCameraPosition);
	D3DXCOLOR ambientColor, ambientColor2;
	ambientColor.r = (float)Timecycle->m_fCurrentAmbientRed;
	ambientColor.g = (float)Timecycle->m_fCurrentAmbientGreen;
	ambientColor.b = (float)Timecycle->m_fCurrentAmbientBlue;
	ambientColor.a = 1.0;
	ambientColor2.r = (float)Timecycle->m_fCurrentAmbientObjRed;
	ambientColor2.g = (float)Timecycle->m_fCurrentAmbientObjGreen;
	ambientColor2.b = (float)Timecycle->m_fCurrentAmbientObjBlue;
	ambientColor2.a = 1.0;
	m_pEffect->SetVector(m_pEffect->m_params.building.Ambient0, (D3DXVECTOR4 *)&ambientColor);
	m_pEffect->SetVector(m_pEffect->m_params.building.Ambient1, (D3DXVECTOR4 *)&ambientColor2);
	m_pEffect->SetFloat(m_pEffect->m_params.building.AmbientMultiplier, 0.3f);
	m_pEffect->SetFloat(m_pEffect->m_params.building.ReflectDir, gGlobalsMgr.g_fReflDir);

	for (unsigned int i = 0; i < RepEntry->header.numMeshes; i++) {
		pMaterial = pMesh->material;
		bHasAlpha = pMesh->vertexAlpha || pMaterial->color.alpha != 255;
		//rwD3D9RenderStateVertexAlphaEnable(bHasAlpha);
		m_pEffect->SetColor(m_pEffect->m_params.building.Color, &pMaterial->color);

		if (CGameIdle::m_pReflectionRT[0] && CGameIdle::m_pReflectionRT[1]){
			m_pEffect->SetTexture("reflTex0", CGameIdle::m_pReflectionRT[0]->GetTex());
			m_pEffect->SetTexture("reflTex1", CGameIdle::m_pReflectionRT[1]->GetTex());
		}
		if (pMaterial->m_pReflection && (pMaterial->surfaceProps.m_dwFlags & 1))
			m_pEffect->SetFloat(m_pEffect->m_params.building.Reflectivity, RpMaterialGetFxEnvShininess(pMaterial));
		else
			m_pEffect->SetFloat(m_pEffect->m_params.building.Reflectivity, 0);
		if (gRenderState != RENDERTYPE_SHADOW) {
			if (pMaterial->texture) {
				if (gRenderState == RENDERTYPE_REFL)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.building.ReflectionTextured);
				else if(gRenderState == RENDERTYPE_FORWARD)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.building.ForwardTextured);
				else if (gRenderState == RENDERTYPE_DEFERRED)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.building.DeferredTextured);

				m_pEffect->SetTexture(m_pEffect->m_params.building.ColorTexture, pMaterial->texture);
				m_pEffect->SetTexture("shadowTex", CShadowMgr::m_pShadowRT[1]->GetTex());
			}
			else if (gRenderState != RENDERTYPE_REFL){
				if (gRenderState == RENDERTYPE_FORWARD)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.building.ForwardColor);
				else if (gRenderState == RENDERTYPE_DEFERRED)
					m_pEffect->SetTechnique(m_pEffect->m_techniques.building.DeferredColor);
			}
			Render(&RepEntry->header, pMesh, m_pEffect);
		}
		else {
			CShadowMgr::m_pEffect->SetFloat(CShadowMgr::m_pEffect->m_params.shadows.Alpha, (float)pMaterial->color.alpha / 255.0f);
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
