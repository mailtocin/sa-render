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

bool FlipTangents(IDirect3DVertexBuffer9 *buffer, UINT ReadSize,UINT ReadOffsetStart,UINT startIndex, UINT numIndices, UINT tangentOffset){
		IDirect3DIndexBuffer9 *pIndexData;
		std::vector < UCHAR > sourceArray;
		sourceArray.resize ( ReadSize );
		UCHAR* pSourceArrayBytes = &sourceArray[0];
		{
				void* pVertexBytesPT = NULL;
				if ( FAILED( buffer->Lock ( ReadOffsetStart, ReadSize, &pVertexBytesPT, D3DLOCK_NOSYSLOCK ) ) )
						return false;
				memcpy ( pSourceArrayBytes, pVertexBytesPT, ReadSize );
				buffer->Unlock ();
		}
		if ( FAILED( g_Device->GetIndices( &pIndexData ) ) )
            return false;

		// Get index buffer data
        std::vector < UCHAR > indexArray;
        indexArray.resize ( ReadSize );
        UCHAR* pIndexArrayBytes = &indexArray[0];
        {
            void* pIndexBytes = NULL;
            if ( FAILED( pIndexData->Lock ( startIndex*2, numIndices*2, &pIndexBytes, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY ) ) )
                return false;
            memcpy ( pIndexArrayBytes, pIndexBytes, numIndices*2 );
            pIndexData->Unlock ();
        }
		for ( UINT i = 0 ; i < numIndices - 2 ; i += 3 )
        {
            // Get triangle vertex indici
            WORD v0 = ((WORD*)pIndexArrayBytes)[ i ];
            WORD v1 = ((WORD*)pIndexArrayBytes)[ i + 1 ];
			WORD v2 = ((WORD*)pIndexArrayBytes)[ i + 2 ];
			// Get vertex positions from original stream
            CVector* pTan0 = (CVector*)( pSourceArrayBytes + v0 * tangentOffset );
            CVector* pTan1 = (CVector*)( pSourceArrayBytes + v1 * tangentOffset );
            CVector* pTan2 = (CVector*)( pSourceArrayBytes + v2 * tangentOffset );
			CVector* pNorm0 = (CVector*)( pSourceArrayBytes + v0 * 12 );
            CVector* pNorm1 = (CVector*)( pSourceArrayBytes + v1 * 12 );
            CVector* pNorm2 = (CVector*)( pSourceArrayBytes + v2 * 12 );
			if ( ( i & 1 ) ){
					(*pTan0).x = -(*pTan0).x;
					(*pTan0).y = -(*pTan0).y;
					(*pTan0).z = -(*pTan0).z;
					(*pTan1).x = -(*pTan1).x;
					(*pTan1).y = -(*pTan1).y;
					(*pTan1).z = -(*pTan1).z;
					(*pTan2).x = -(*pTan2).x;
					(*pTan2).y = -(*pTan2).y;
					(*pTan2).z = -(*pTan2).z;

					(*pNorm0).x = -(*pNorm0).x;
					(*pNorm0).y = -(*pNorm0).y;
					(*pNorm0).z = -(*pNorm0).z;
					(*pNorm1).x = -(*pNorm1).x;
					(*pNorm1).y = -(*pNorm1).y;
					(*pNorm1).z = -(*pNorm1).z;
					(*pNorm2).x = -(*pNorm2).x;
					(*pNorm2).y = -(*pNorm2).y;
					(*pNorm2).z = -(*pNorm2).z;
			}
		}
		return true;
}

void CVehicleRender::RenderCB(RwResEntry *repEntry, RpAtomic *atomic, unsigned char type, char flags)
{
	D3DXVECTOR4 sun;
	D3DXVECTOR4 cam;
	D3DXCOLOR ambientColor,ambientColor2;
	D3DXCOLOR vehicleColor;
	CVector camPos;
	RxD3D9InstanceData *mesh;
	UINT passes;
	D3DXMATRIX world;
	D3DXMATRIX worldViewProj,lightProj,sunMatrix,lightView,vp,proj,worldtransp,viewinv,view,wv;
	IDirect3DBaseTexture9 *diffuse,*bump,*specular;
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
			g_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			g_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			rwD3D9DrawIndexedPrimitive(repEntry->header.primType, mesh->baseIndex, 0, mesh->numVertices,
				mesh->startIndex, mesh->numPrimitives);
			m_pEffect->EndPass();
		}
		else
			rwD3D9DrawPrimitive(repEntry->header.primType, mesh->baseIndex, mesh->numPrimitives);
		m_pEffect->End();
		mesh++;
	}
}