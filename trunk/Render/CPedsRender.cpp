#include "CPedsRender.h"
#include "CVehicleRender.h"
#include "CTextureMaps.h"
#include "CRender.h"
#include "RenderWare.h"
#include "CPatch.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "CGame.h"

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
	if(errors)
	{
		MessageBox(0, (char *)errors->GetBufferPointer(), 0, 0);
		errors->Release();
	}
	if(FAILED(result))
	{
		MessageBox(0, "CObjectRender::Setup: D3DXCreateEffectFromFile() - failed while compiling object.fx", 0, 0);
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
	D3DXVECTOR4 sun;
	D3DXVECTOR4 cam;
	D3DXCOLOR ambientColor,ambientColor2;
	IDirect3DBaseTexture9 *diffuse,*bump,*specular;
	UINT passes;
	D3DXMATRIX world;
	D3DXMATRIX worldViewProj,lightProj,sunMatrix,vp,proj,worldtransp,lightView,view,wv;
	HRESULT result;
	D3DXVECTOR4 constData[224];
	g_Device->GetVertexShaderConstantF(5,(float*)constData,224);
	m_pEffect->SetVectorArray("constdata",constData,224);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	g_Device->GetTransform(D3DTS_VIEW,&view);
	rwD3D9VSGetWorldNormalizedTransposeMatrix(&world);
	rwD3D9VSGetWorldViewTransposedMatrix(&wv);
	rwD3D9VSGetInverseWorldMatrix((void*)worldtransp);
	D3DXMatrixInverse(&worldtransp,NULL,&worldtransp);
	D3DXMatrixTranspose(&worldtransp,&worldtransp);
	D3DXMatrixMultiplyTranspose(&vp,&view,&proj);
	D3DXMatrixMultiply(&worldViewProj,&vp,&worldtransp);
	GetSunPosn((CVector *)&sun);
	sun.w = 1.0;
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
	if (*rwD3D9LastPixelShaderUsed)
	{
		*rwD3D9LastPixelShaderUsed = NULL;
		g_Device->SetPixelShader(NULL);
	}
	if (instData->material->texture)
	{
		STexture *texture = (STexture *)instData->material->texture;
		rwD3D9SetTexture(texture, 0);
		g_Device->GetTexture(0,&diffuse);
		m_pEffect->SetTexture("gtDiffuse",diffuse);
		if(texture){
				if(texture->m_pNormalMap)
				{
						rwD3D9SetTexture(texture->m_pNormalMap, 1);
						g_Device->GetTexture(1,&bump);
						m_pEffect->SetTexture("gtNormals",bump);
						if(texture->m_pSpecMap) {
								rwD3D9SetTexture(texture->m_pSpecMap, 2);
								g_Device->GetTexture(2,&specular);
								m_pEffect->SetTexture("gtSpecular",specular);
						} else {
								m_pEffect->SetTexture("gtSpecular",CVehicleRender::defspec);
						}
				} else {
						if(texture->m_pSpecMap) {
								rwD3D9SetTexture(texture->m_pSpecMap, 2);
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
	{
		rwD3D9SetTexture(0, 0);
	}
	if(*rwD3D9LastVertexShaderUsed != instData->vertexShader)
	{
		*rwD3D9LastVertexShaderUsed = (IDirect3DVertexShader9*)instData->vertexShader;
		g_Device->SetVertexShader((IDirect3DVertexShader9*)instData->vertexShader);
	}
	if(resEntry->indexBuffer)
	{
		rwD3D9RenderStateFlushCache();
		m_pEffect->Begin(&passes,0);
		m_pEffect->BeginPass(0);
		g_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		result = g_Device->DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntry->primType, instData->baseIndex, 0, instData->numVertices, instData->startIndex, instData->numPrimitives);
		m_pEffect->EndPass();
		m_pEffect->End();
	}
	else
	{
		rwD3D9RenderStateFlushCache();
		result = g_Device->DrawPrimitive((D3DPRIMITIVETYPE)resEntry->primType, instData->baseIndex, instData->numPrimitives);
	}
	return result;
}