#include "CPedsRender.h"
#include "CVehicleRender.h"
#include "CTextureMaps.h"
#include "CRender.h"
#include "RenderWare.h"
#include "CPatch.h"
#include "CDebug.h"
#include <d3d9.h>

ID3DXEffect *CPedsRender::m_pEffect;
D3DXMATRIX CPedsRender::m_LightViewProj;
IDirect3DPixelShader9 **rwD3D9LastPixelShaderUsed = (IDirect3DPixelShader9 **)0x8E244C;
IDirect3DVertexShader9 **rwD3D9LastVertexShaderUsed = (IDirect3DVertexShader9 **)0x8E2448;

void CPedsRender::Patch()
{
	CPatch::SetPointer(0x7CB276, rxD3D9VertexShaderDefaultMeshRenderCallBack);
}

bool CPedsRender::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"skin.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors, "CPedsRender", "skin", result))
	{
		return false;
	}
	return true;
}

void CPedsRender::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}

void CPedsRender::Lost()
{
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}

void rwD3D9VSGetInverseWorldMatrix(void *inverseWorldMatrix) {
	typedef void (*funcPtr)(void *);
	funcPtr rwD3D9VSGetInverseWorldMatrix = (funcPtr)(0x007647B0);

	rwD3D9VSGetInverseWorldMatrix(inverseWorldMatrix);
}

HRESULT __cdecl CPedsRender::rxD3D9VertexShaderDefaultMeshRenderCallBack(RxD3D9ResEntryHeader *resEntry, RxD3D9InstanceData *instData)
{
	DWORD oDB,oSB,oBO,oAB,oAT;
	UINT passes;
	D3DXMATRIX world;
	D3DXCOLOR color;
	D3DXMATRIX worldViewProj,vp,proj,worldtransp,view;
	HRESULT result;
	D3DXVECTOR4 constData[224];
	GetCurrentStates(&oDB,&oSB,&oBO,&oAB,&oAT);
	g_Device->GetVertexShaderConstantF(5,(float*)constData,224);
	m_pEffect->SetVectorArray("constdata",constData,224);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	g_Device->GetTransform(D3DTS_VIEW,&view);
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	rwD3D9VSGetInverseWorldMatrix((void*)worldtransp);
	D3DXMatrixInverse(&worldtransp,NULL,&worldtransp);
	D3DXMatrixTranspose(&worldtransp,&worldtransp);
	D3DXMatrixMultiplyTranspose(&vp,&view,&proj);
	D3DXMatrixMultiply(&worldViewProj,&vp,&worldtransp);
	//rwD3D9RenderStateVertexAlphaEnable(instData->vertexAlpha || instData->material->color.alpha != 255);
	m_pEffect->SetMatrix("gmWorldViewProj",&worldViewProj);
	m_pEffect->SetMatrix("gmWorld",&world);
	color.r = (float)instData->material->color.red / 255.0f;
	color.g = (float)instData->material->color.green / 255.0f;
	color.b = (float)instData->material->color.blue / 255.0f;
	color.a = (float)instData->material->color.alpha / 255.0f;
	m_pEffect->SetFloat("gfSpecularFactor", (float)instData->material->m_pReflection->m_ucIntensity* 0.0039215686f);
	m_pEffect->SetVector("gvColor", (D3DXVECTOR4 *)&color);
	if (instData->material->texture)
	{
		CRender::SetTextureMaps((STexture*)instData->material->texture,m_pEffect);
	}
	m_pEffect->Begin(&passes,0);
	m_pEffect->BeginPass(0);
	if(resEntry->indexBuffer)
	{
		result = g_Device->DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntry->primType, instData->baseIndex, 0, instData->numVertices, instData->startIndex, instData->numPrimitives);
	}
	else
	{
		result = g_Device->DrawPrimitive((D3DPRIMITIVETYPE)resEntry->primType, instData->baseIndex, instData->numPrimitives);
	}
	m_pEffect->EndPass();
	m_pEffect->End();
	SetOldStates(oDB,oSB,oBO,oAB,oAT);
	return result;
}