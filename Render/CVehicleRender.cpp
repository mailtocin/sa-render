#include "CVehicleRender.h"
#include "CTextureMaps.h"
#include "CRender.h"
#include "RenderWare.h"
#include "CPatch.h"
#include "D3D9Headers\d3d9.h"
#include "CGame.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>

ID3DXEffect *CVehicleRender::m_pEffect;
D3DXMATRIX CVehicleRender::m_LightViewProj;
IDirect3DTexture9 *CVehicleRender::defnormal;
IDirect3DTexture9 *CVehicleRender::defspec;

void CVehicleRender::Patch()
{
	CPatch::SetPointer(0x5D9FE4, RenderCB);
}

bool CVehicleRender::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"vehicle.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	D3DXCreateTextureFromFile(g_Device,"normal.tga",&defnormal);
	D3DXCreateTextureFromFile(g_Device,"spec.tga",&defspec);
	if(errors)
	{
		MessageBox(0, (char *)errors->GetBufferPointer(), 0, 0);
		errors->Release();
	}
	if(FAILED(result))
	{
		MessageBox(0, "CVehicleRender::Setup: D3DXCreateEffectFromFile() - failed while compiling vechicle.fx", 0, 0);
		return false;
	}
	return true;
}

void CVehicleRender::Update()
{
	
}

void CVehicleRender::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}

void CVehicleRender::Lost()
{
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}

void CVehicleRender::RenderCB(RwResEntry *repEntry, RpAtomic *atomic, unsigned char type, char flags)
{
	D3DXVECTOR4 sun;
	DWORD oDB,oSB,oBO,oAB,oAT;
	D3DXVECTOR4 cam;
	D3DXCOLOR ambientColor,ambientColor2;
	D3DXCOLOR vehicleColor;
	CVector camPos;
	RxD3D9InstanceData *mesh;
	UINT passes;
	D3DXMATRIX world;
	D3DXMATRIX worldViewProj,lightProj,sunMatrix,lightView,vp,proj,worldtransp,viewinv,view,wv,wvi;
	IDirect3DBaseTexture9 *diffuse,*bump,*specular;
	g_Device->GetRenderState(D3DRS_DESTBLEND,&oDB);
	g_Device->GetRenderState(D3DRS_SRCBLEND,&oSB);
	g_Device->GetRenderState(D3DRS_BLENDOP,&oBO);
	g_Device->GetRenderState(D3DRS_ALPHABLENDENABLE,&oAB);
	g_Device->GetRenderState(D3DRS_ALPHATESTENABLE,&oAT);
	RpGeometry *geometry = atomic->geometry;
	unsigned int geometryFlags = geometry->flags;
	rwD3D9EnableClippingIfNeeded(atomic, type);
	if(repEntry->header.indexBuffer)
		rwD3D9SetIndices(repEntry->header.indexBuffer);
	rwD3D9SetStreams(repEntry->header.vertexStream, repEntry->header.useOffsets);
	rwD3D9SetVertexDeclaration(repEntry->header.vertexDeclaration);
	Update();
	rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(atomic->object.object.parent));
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	rwD3D9VSGetWorldViewTransposedMatrix(&wv);
	getWorldViewProj(&worldViewProj,RwFrameGetLTM(atomic->object.object.parent),&vp);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	g_Device->GetTransform(D3DTS_VIEW,&view);
	D3DXMatrixInverse(&viewinv,0,&view);
	RwMatrix* ltm = RwFrameGetLTM(atomic->object.object.parent);
	worldtransp.m[0][0] = ltm->right.x;
	worldtransp.m[0][1] = ltm->up.x;
	worldtransp.m[0][2] = ltm->at.x;
	worldtransp.m[0][3] = ltm->pos.x;

	worldtransp.m[1][0] = ltm->right.y;
	worldtransp.m[1][1] = ltm->up.y;
	worldtransp.m[1][2] = ltm->at.y;
	worldtransp.m[1][3] = ltm->pos.y;

	worldtransp.m[2][0] = ltm->right.z;
	worldtransp.m[2][1] = ltm->up.z;
	worldtransp.m[2][2] = ltm->at.z;
	worldtransp.m[2][3] = ltm->pos.z;

	worldtransp.m[3][0] = 0.0f;
	worldtransp.m[3][1] = 0.0f;
	worldtransp.m[3][2] = 0.0f;
	worldtransp.m[3][3] = 1.0f;
	FindPlayerCoors(&camPos, 0);
	GetSunPosn((CVector *)&sun,50);
	sun.w = 1.0;
	sun.x += camPos.x;
	sun.y += camPos.y;
	sun.z += camPos.z;
	m_pEffect->SetMatrix("gmWorldViewProj",&worldViewProj);
	m_pEffect->SetMatrix("gmWorld",&worldtransp);
	m_pEffect->SetMatrix("gmWorldView",&wv);
	D3DXMatrixInverse(&wvi,NULL,&wv);
	m_pEffect->SetMatrixTranspose("gmWorldViewIT",&wvi);
	m_pEffect->SetMatrix("gmViewInv",&viewinv);
	m_pEffect->SetVector("gvDirLight", &sun);
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
	memcpy(&cam, GetCamPos(), 12);
	cam.w = 1.0;
	m_pEffect->SetVector("gvEye", &cam);
	mesh = &repEntry->meshData;
	// Рендерим все меши
	for(unsigned int i = 0; i < repEntry->header.numMeshes; i++)
	{
		rwD3D9RenderStateVertexAlphaEnable(mesh->vertexAlpha || mesh->material->color.alpha != 255);

		vehicleColor.r = (float)mesh->material->color.red / 255.0f;
		vehicleColor.g = (float)mesh->material->color.green / 255.0f;
		vehicleColor.b = (float)mesh->material->color.blue / 255.0f;
		vehicleColor.a = (float)mesh->material->color.alpha / 255.0f;
		m_pEffect->SetVector("gvPaintColor", (D3DXVECTOR4 *)&vehicleColor);
		if(flags & (rpGEOMETRYTEXTURED2|rpGEOMETRYTEXTURED)) {
			rwD3D9SetTexture(mesh->material->texture, 0);
			STexture* tex = (STexture*)mesh->material->texture;
			g_Device->GetTexture(0,&diffuse);
			m_pEffect->SetTexture("gtDiffuse",diffuse);
			if(tex){
					if(tex->m_pNormalMap)
					{
						rwD3D9SetTexture(tex->m_pNormalMap, 1);
						g_Device->GetTexture(1,&bump);
						m_pEffect->SetTexture("gtNormals",bump);
						if(tex->m_pSpecMap) {
								rwD3D9SetTexture(tex->m_pSpecMap, 2);
								g_Device->GetTexture(2,&specular);
								m_pEffect->SetTexture("gtSpecular",specular);
						} else {
								m_pEffect->SetTexture("gtSpecular",defspec);
						}
					} else {
							m_pEffect->SetTexture("gtNormals",defnormal);
							if(tex->m_pSpecMap) {
									rwD3D9SetTexture(tex->m_pSpecMap, 2);
									g_Device->GetTexture(2,&specular);
									m_pEffect->SetTexture("gtSpecular",specular);
							} else {
									m_pEffect->SetTexture("gtSpecular",defspec);
							}
					}
			}
		} else {
			rwD3D9SetTexture(NULL, 0);
		}
		m_pEffect->Begin(&passes,0);
		if(repEntry->header.indexBuffer)
		{
			m_pEffect->BeginPass(0);
			rwD3D9DrawIndexedPrimitive(repEntry->header.primType, mesh->baseIndex, 0, mesh->numVertices,
				mesh->startIndex, mesh->numPrimitives);
			m_pEffect->EndPass();
		}
		else
			rwD3D9DrawPrimitive(repEntry->header.primType, mesh->baseIndex, mesh->numPrimitives);
		m_pEffect->End();
		mesh++;
	}
	g_Device->SetRenderState(D3DRS_DESTBLEND,oDB);
	g_Device->SetRenderState(D3DRS_SRCBLEND,oSB);
	g_Device->SetRenderState(D3DRS_BLENDOP,oBO);
	g_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,oAB);
	g_Device->SetRenderState(D3DRS_ALPHATESTENABLE,oAT);
}