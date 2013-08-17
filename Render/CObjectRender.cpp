#include "CObjectRender.h"
#include "CTextureMaps.h"
#include "CRender.h"
#include "RenderWare.h"
#include "CPatch.h"
#include "D3D9Headers\d3d9.h"
#include "CDebug.h"

ID3DXEffect *CObjectRender::m_pEffect;
RpAtomic *CObjectRender::g_DefaultRender_Atomic = NULL;
RwResEntry *CObjectRender::g_DefaultRender_ResEntry = NULL;
int CObjectRender::g_DefaultRender_ClipSphere = NULL;
int CObjectRender::g_DefaultRender_Flags = NULL;

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
	DWORD oDB,oSB,oBO,oAB,oAT;
	UINT passes;
	D3DXCOLOR color;
	D3DXMATRIX vp,worldTransposedMatrix,worldViewProj,world;
	GetCurrentStates(&oDB,&oSB,&oBO,&oAB,&oAT);
	//rwD3D9RenderStateVertexAlphaEnable(g_DefaultRender_ResEntry->meshData.vertexAlpha || g_DefaultRender_ResEntry->meshData.material->color.alpha != 255);
	if(g_DefaultRender_Flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)){
		CRender::SetTextureMaps((STexture*)g_DefaultRender_ResEntry->meshData.material->texture,m_pEffect);
	}
	rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent));
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	getWorldViewProj(&worldViewProj,RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent),&vp);
	m_pEffect->SetMatrix("gmWorldViewProj",&worldViewProj);
	m_pEffect->SetMatrix("gmWorld",&world);
	m_pEffect->SetBool("vcol",false);
	color.r = (float)g_DefaultRender_ResEntry->meshData.material->color.red / 255.0f;
	color.g = (float)g_DefaultRender_ResEntry->meshData.material->color.green / 255.0f;
	color.b = (float)g_DefaultRender_ResEntry->meshData.material->color.blue / 255.0f;
	color.a = (float)g_DefaultRender_ResEntry->meshData.material->color.alpha / 255.0f;
	m_pEffect->SetVector("gvColor", (D3DXVECTOR4 *)&color);
	m_pEffect->SetFloat("gfSpecularFactor", 1.0f-(float)g_DefaultRender_ResEntry->meshData.material->m_pReflection->m_ucIntensity* 0.0039215686f);
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
	DWORD oDB,oSB,oBO,oAB,oAT;
	UINT passes;
	D3DXCOLOR color;
	D3DXMATRIX vp,worldViewProj,world;
	GetCurrentStates(&oDB,&oSB,&oBO,&oAB,&oAT);
	//rwD3D9RenderStateVertexAlphaEnable(g_DefaultRender_ResEntry->meshData.vertexAlpha || g_DefaultRender_ResEntry->meshData.material->color.alpha != 255);
	if(g_DefaultRender_Flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)){
		CRender::SetTextureMaps((STexture*)g_DefaultRender_ResEntry->meshData.material->texture,m_pEffect);
	}
	RwMatrix* ltm = RwFrameGetLTM(g_DefaultRender_Atomic->object.object.parent);
	rwD3D9VSSetActiveWorldMatrix(ltm);
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	getWorldViewProj(&worldViewProj,ltm,&vp);
	m_pEffect->SetMatrix("gmWorldViewProj",&worldViewProj);
	m_pEffect->SetMatrix("gmWorld",&world);
	m_pEffect->SetBool("vcol",false);
	color.r = (float)g_DefaultRender_ResEntry->meshData.material->color.red / 255.0f;
	color.g = (float)g_DefaultRender_ResEntry->meshData.material->color.green / 255.0f;
	color.b = (float)g_DefaultRender_ResEntry->meshData.material->color.blue / 255.0f;
	color.a = (float)g_DefaultRender_ResEntry->meshData.material->color.alpha / 255.0f;
	m_pEffect->SetFloat("gfSpecularFactor", 1.0f-(float)g_DefaultRender_ResEntry->meshData.material->m_pReflection->m_ucIntensity* 0.0039215686f);
	m_pEffect->SetVector("gvColor", (D3DXVECTOR4 *)&color);
	m_pEffect->Begin(&passes,0);
	m_pEffect->BeginPass(0);
	device->DrawIndexedPrimitive(Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	m_pEffect->EndPass();
	m_pEffect->End();
	return SetOldStates(oDB,oSB,oBO,oAB,oAT);
}

void __cdecl CObjectRender::NvcRenderCB(RwResEntry *repEntry, RpAtomic *object, unsigned __int8 type, char flags)
{
	DWORD oDB,oSB,oBO,oAB,oAT;
	RxD3D9ResEntryHeader *header;
	RxD3D9InstanceData *mesh;
	RpMaterial *mat;
	UINT passes;
	D3DXMATRIX worldViewProj,vp,world;
	D3DXCOLOR color;
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
	RwMatrix* ltm = RwFrameGetLTM(object->object.object.parent);
	rwD3D9VSSetActiveWorldMatrix(ltm);
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	getWorldViewProj(&worldViewProj,ltm,&vp);
	m_pEffect->SetMatrix("gmWorldViewProj",&worldViewProj);
	m_pEffect->SetBool("vcol",true);
	m_pEffect->SetMatrix("gmWorld",&world);
	for(unsigned int i = 0; i < repEntry->header.numMeshes; i++)
	{
		mat = mesh->material;
		//rwD3D9RenderStateVertexAlphaEnable(mesh->vertexAlpha || mat->color.alpha != 255);
		color.r = (float)mat->color.red / 255.0f;
		color.g = (float)mat->color.green / 255.0f;
		color.b = (float)mat->color.blue / 255.0f;
		color.a = (float)mat->color.alpha / 255.0f;
		m_pEffect->SetFloat("gfSpecularFactor", 1.0f-(float)mat->m_pReflection->m_ucIntensity* 0.0039215686f);
		m_pEffect->SetVector("gvColor", (D3DXVECTOR4 *)&color);
		if(flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)){
			CRender::SetTextureMaps((STexture*)mat->texture,m_pEffect);
		}
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