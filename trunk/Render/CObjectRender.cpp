#include "CObjectRender.h"
#include "CVehicleRender.h"
#include "CTextureMaps.h"
#include "CRender.h"
#include "RenderWare.h"
#include "CPatch.h"
#include "D3D9Headers\d3d9.h"
#include "CGame.h"
#include "CDebug.h"

ID3DXEffect *CObjectRender::m_pEffect;
D3DXMATRIX CObjectRender::m_LightViewProj;
D3DXMATRIX CObjectRender::m_LightViewProj2;
bool CObjectRender::bWeaponRender;
RpAtomic *CObjectRender::g_DefaultRender_Atomic = NULL;
RwResEntry *CObjectRender::g_DefaultRender_ResEntry = NULL;
int CObjectRender::g_DefaultRender_ClipSphere = NULL;
int CObjectRender::g_DefaultRender_Flags = NULL;
void (__cdecl *rxD3D9DefaultRenderCallback)(RwResEntry *resEntry, RpAtomic *atomic, int bClipSphere, int flags) = (void (__cdecl *)(RwResEntry *, RpAtomic *, int, int)) 0x756DF0;

void CObjectRender::Patch()
{
	CPatch::SetPointer(0x5D67F4, NvcRenderCB);
	CPatch::Nop(0x75703B, 1);
	CPatch::RedirectCall(0x75703C, DefaultRender_DrawIndexedPrimitiveA);
	CPatch::Nop(0x75705C, 1);
	CPatch::RedirectCall(0x75705D, DefaultRender_DrawPrimitiveA);
	CPatch::Nop(0x757315, 1);
	CPatch::RedirectCall(0x757316, DefaultRender_DrawIndexedPrimitiveB);
	CPatch::Nop(0x757336, 1);
	CPatch::RedirectCall(0x757337, DefaultRender_DrawPrimitiveB);
	CPatch::SetPointer(0x7578AE,DefaultRender_Callback);
	CPatch::SetPointer(0x75E55E,DefaultRender_Callback);
	CPatch::SetPointer(0x7CB24B,DefaultRender_Callback);
}

bool CObjectRender::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"object.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CObjectRender", "object", result))
	{
		return false;
	}
	return true;
}

void CObjectRender::Update()
{
	
}

void CObjectRender::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}

void CObjectRender::Lost()
{
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}

void CObjectRender::DefaultRender_Callback(RwResEntry *resEntry, RpAtomic *atomic, int bClipSphere, int flags)
{
 g_DefaultRender_Atomic = atomic;
 g_DefaultRender_ResEntry = resEntry;
 g_DefaultRender_ClipSphere = bClipSphere;
 g_DefaultRender_Flags = flags;
 rxD3D9DefaultRenderCallback(resEntry, atomic, bClipSphere, flags);
}

HRESULT __fastcall CObjectRender::DefaultRender_DrawIndexedPrimitiveB(int ecx0,
																	int edx0,
												IDirect3DDevice9 *device,
												D3DPRIMITIVETYPE Type,
												INT BaseVertexIndex,
												UINT MinIndex,
												UINT NumVertices,
												UINT StartIndex,
												UINT PrimitiveCount)
{
	IDirect3DBaseTexture9* diffuse;
	DWORD oDB,oSB,oBO,oAB,oAT;
	D3DXVECTOR4 sun;
	D3DXVECTOR4 cam;
	D3DXCOLOR ambientColor,ambientColor2;
	UINT passes;
	D3DXMATRIX v,p,vp,worldTransposedMatrix,worldViewProj,world,wv;
	GetCurrentStates(&oDB,&oSB,&oBO,&oAB,&oAT);
	if(g_DefaultRender_Flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)){
		device->GetTexture(0,&diffuse);
		m_pEffect->SetTexture("gtDiffuse",diffuse);
	}

	RwMatrix* ltm = RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent);
	worldTransposedMatrix.m[0][0] = ltm->right.x;
	worldTransposedMatrix.m[0][1] = ltm->up.x;
	worldTransposedMatrix.m[0][2] = ltm->at.x;
	worldTransposedMatrix.m[0][3] = ltm->pos.x;

	worldTransposedMatrix.m[1][0] = ltm->right.y;
	worldTransposedMatrix.m[1][1] = ltm->up.y;
	worldTransposedMatrix.m[1][2] = ltm->at.y;
	worldTransposedMatrix.m[1][3] = ltm->pos.y;

	worldTransposedMatrix.m[2][0] = ltm->right.z;
	worldTransposedMatrix.m[2][1] = ltm->up.z;
	worldTransposedMatrix.m[2][2] = ltm->at.z;
	worldTransposedMatrix.m[2][3] = ltm->pos.z;

	worldTransposedMatrix.m[3][0] = 0.0f;
	worldTransposedMatrix.m[3][1] = 0.0f;
	worldTransposedMatrix.m[3][2] = 0.0f;
	worldTransposedMatrix.m[3][3] = 1.0f;
	
	rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent));
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	rwD3D9VSGetWorldViewTransposedMatrix(&wv);
	getWorldViewProj(&worldViewProj,RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent),&vp);
	m_pEffect->SetMatrix("gmWorldViewProj",&worldViewProj);
	m_pEffect->SetMatrix("gmWorld",&world);
	m_pEffect->SetMatrix("gmWorldView",&wv);
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
	m_pEffect->Begin(&passes,0);
	m_pEffect->BeginPass(0);
	device->DrawIndexedPrimitive(Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	m_pEffect->EndPass();
	m_pEffect->End();
	return SetOldStates(oDB,oSB,oBO,oAB,oAT);
}
HRESULT __fastcall CObjectRender::DefaultRender_DrawPrimitiveB(int ecx0,
                                 int edx0,
         IDirect3DDevice9 *device,
         D3DPRIMITIVETYPE PrimitiveType,
         UINT StartVertex,
         UINT PrimitiveCount)
{
 return device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}
HRESULT __fastcall CObjectRender::DefaultRender_DrawPrimitiveA(int ecx0,
                                 int edx0,
         IDirect3DDevice9 *device,
         D3DPRIMITIVETYPE PrimitiveType,
         UINT StartVertex,
         UINT PrimitiveCount)
{
	return device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT __fastcall CObjectRender::DefaultRender_DrawIndexedPrimitiveA(int ecx0,
																	int edx0,
												IDirect3DDevice9 *device,
												D3DPRIMITIVETYPE Type,
												INT BaseVertexIndex,
												UINT MinIndex,
												UINT NumVertices,
												UINT StartIndex,
												UINT PrimitiveCount)
{
	IDirect3DBaseTexture9* diffuse;
	DWORD oDB,oSB,oBO,oAB,oAT;
	D3DXVECTOR4 sun;
	D3DXVECTOR4 cam;
	D3DXCOLOR ambientColor,ambientColor2;
	UINT passes;
	D3DXMATRIX v,p,vp,worldTransposedMatrix,worldViewProj,world,wv;
	GetCurrentStates(&oDB,&oSB,&oBO,&oAB,&oAT);
	if(g_DefaultRender_Flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)){
		RwTexture *texture = g_DefaultRender_Atomic->repEntry->meshData.material->texture;
		device->GetTexture(0,&diffuse);
		m_pEffect->SetTexture("gtDiffuse",diffuse);
	}

	RwMatrix* ltm = RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent);
	worldTransposedMatrix.m[0][0] = ltm->right.x;
	worldTransposedMatrix.m[0][1] = ltm->up.x;
	worldTransposedMatrix.m[0][2] = ltm->at.x;
	worldTransposedMatrix.m[0][3] = ltm->pos.x;

	worldTransposedMatrix.m[1][0] = ltm->right.y;
	worldTransposedMatrix.m[1][1] = ltm->up.y;
	worldTransposedMatrix.m[1][2] = ltm->at.y;
	worldTransposedMatrix.m[1][3] = ltm->pos.y;

	worldTransposedMatrix.m[2][0] = ltm->right.z;
	worldTransposedMatrix.m[2][1] = ltm->up.z;
	worldTransposedMatrix.m[2][2] = ltm->at.z;
	worldTransposedMatrix.m[2][3] = ltm->pos.z;

	worldTransposedMatrix.m[3][0] = 0.0f;
	worldTransposedMatrix.m[3][1] = 0.0f;
	worldTransposedMatrix.m[3][2] = 0.0f;
	worldTransposedMatrix.m[3][3] = 1.0f;
	
	rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent));
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	rwD3D9VSGetWorldViewTransposedMatrix(&wv);
	getWorldViewProj(&worldViewProj,RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent),&vp);
	m_pEffect->SetMatrix("gmWorldViewProj",&worldViewProj);
	m_pEffect->SetMatrix("gmWorld",&world);
	m_pEffect->SetMatrix("gmWorldView",&wv);
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
	m_pEffect->Begin(&passes,0);
	m_pEffect->BeginPass(0);
	device->DrawIndexedPrimitive(Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	m_pEffect->EndPass();
	m_pEffect->End();
	return SetOldStates(oDB,oSB,oBO,oAB,oAT);
}

void __cdecl CObjectRender::NvcRenderCB(RwResEntry *repEntry, RpAtomic *object, unsigned __int8 type, char flags)
{
	D3DXVECTOR4 sun;
	DWORD oDB,oSB,oBO,oAB,oAT;
	D3DXVECTOR4 cam;
	D3DXCOLOR ambientColor,ambientColor2;
	RxD3D9ResEntryHeader *header;
	RxD3D9InstanceData *mesh;
	RpMaterial *mat;
	IDirect3DBaseTexture9 *diffuse,*bump,*specular;
	CVector camPos;
	UINT passes;
	D3DXMATRIX world;
	D3DXMATRIX worldViewProj,lightProj,sunMatrix,vp,proj,worldtransp,lightView,wv;
	GetCurrentStates(&oDB,&oSB,&oBO,&oAB,&oAT);
	rwD3D9EnableClippingIfNeeded(object, type);
	rwD3D9SetRenderState(60, 0xFF000000u); // ?
	header = &repEntry->header;
	mesh = &repEntry->meshData;
	if(repEntry->header.indexBuffer)
		rwD3D9SetIndices(repEntry->header.indexBuffer);
	rwD3D9SetStreams(repEntry->header.vertexStream, repEntry->header.useOffsets);
	rwD3D9SetVertexDeclaration(repEntry->header.vertexDeclaration);
	CDebug::StoreVertexDeclaration((IDirect3DVertexDeclaration9*)repEntry->header.vertexDeclaration);
	rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(object->object.object.parent));
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	rwD3D9VSGetWorldViewTransposedMatrix(&wv);
	getWorldViewProj(&worldViewProj,RwFrameGetLTM(object->object.object.parent),&vp);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	RwMatrix* ltm = RwFrameGetLTM(object->object.object.parent);
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

	m_pEffect->SetMatrix("gmWorld",&world);
	m_pEffect->SetMatrix("gmWorldView",&wv);
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
	for(unsigned int i = 0; i < repEntry->header.numMeshes; i++)
	{
		mat = mesh->material;
		rwD3D9RenderStateVertexAlphaEnable(mesh->vertexAlpha || mesh->material->color.alpha != 255);
		if(flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)){
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
								m_pEffect->SetTexture("gtSpecular",CVehicleRender::defspec);
						}
					} else {
							if(tex->m_pSpecMap) {
								rwD3D9SetTexture(tex->m_pSpecMap, 2);
								g_Device->GetTexture(2,&specular);
								m_pEffect->SetTexture("gtSpecular",specular);
							} else {
								m_pEffect->SetTexture("gtSpecular",CVehicleRender::defspec);
							}
							m_pEffect->SetTexture("gtNormals",CVehicleRender::defnormal);
					}
			}

		}
		else
			rwD3D9SetTexture(NULL, 0);
		m_pEffect->Begin(&passes,0);
		if(header->indexBuffer){
			m_pEffect->BeginPass(0);
			rwD3D9DrawIndexedPrimitive(header->primType, mesh->baseIndex, 0, mesh->numVertices, mesh->startIndex, mesh->numPrimitives);
			m_pEffect->EndPass();
		}
		else
			rwD3D9DrawPrimitive(header->primType, mesh->baseIndex, mesh->numPrimitives);
		m_pEffect->End();
		++mesh;
	}
	SetOldStates(oDB,oSB,oBO,oAB,oAT);
}