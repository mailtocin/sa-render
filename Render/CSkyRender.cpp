#include "CSkyRender.h"
#include "CGame.h"
#include "CDebug.h"

ID3DXEffect *CSkyRender::m_pEffect;
IDirect3DTexture9 *CSkyRender::CloudTex;
D3DXMATRIX CSkyRender::gm_WorldViewProjection;
D3DXMATRIX CSkyRender::gm_World;
LPD3DXMESH CSkyRender::skySphere;

bool CSkyRender::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result = D3DXCreateEffectFromFile(g_Device,"resources/Shaders/sky.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForShaderErrors(errors,"CSkyRender","sky.fx",result))
	{
		return false;
	}
	D3DXCreateTextureFromFile(g_Device,"resources/Textures/clouds.tga",&CloudTex);
	return true;
}
struct _VERTEX {
	D3DXVECTOR3 pos;     // vertex position
	D3DXVECTOR3 norm;    // vertex normal
	float tu;            // texture coordinates
	float tv;
} VERTEX,*LPVERTEX;

#define FVF_VERTEX    D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1
bool CSkyRender::CreateSkySphere(float fRad,UINT slices,UINT stacks) {
	// create the sphere
	LPD3DXMESH mesh;
	if (FAILED(D3DXCreateSphere(g_Device,fRad,slices,stacks,&mesh,NULL)))
		return false;
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM,FVF_VERTEX,g_Device,&skySphere)))
		return false;
	// finished with the original mesh, release it
	mesh->Release();
	// lock the vertex buffer
	_VERTEX* pVerts = NULL;
	if (SUCCEEDED(skySphere->LockVertexBuffer(0,(VOID **) &pVerts))) {
		// get vertex count
		int numVerts=skySphere->GetNumVertices();

		// loop through the vertices
		for (int i=0;i<numVerts;i++) {
			pVerts->tu=asinf(pVerts->norm.x)/D3DX_PI+0.5f;
			pVerts->tv=asinf(pVerts->norm.y)/D3DX_PI+0.5f;
			pVerts++;
		}
		// unlock the vertex buffer
		skySphere->UnlockVertexBuffer();
	}
	return true;
}
void CSkyRender::PreRender(D3DXVECTOR4 *pos,D3DXMATRIX *viewproj)
{
	g_Device->EndScene();
	if (!skySphere)
	{
		CreateSkySphere(Timecycle->m_fCurrentFarClip*0.9f,100,100);
	}
	g_Device->BeginScene();
	D3DXMATRIX meshRotate, meshTranslate;
	D3DXMatrixRotationY(&meshRotate, D3DXToRadian(0));
	D3DXMatrixTranslation(&meshTranslate, pos->x,pos->y,pos->z);
	D3DXMatrixMultiply(&gm_World, &meshRotate, &meshTranslate);
	D3DXMatrixMultiplyTranspose(&gm_WorldViewProjection, &gm_World, viewproj);
}
void CSkyRender::Render(D3DXVECTOR4 *lightDirection)
{
	UINT passes;
	m_pEffect->SetTechnique("Sky");
	m_pEffect->SetTexture("cloudTex",CloudTex);
	m_pEffect->SetVector("lightDirection",lightDirection);
	m_pEffect->SetVector("skyColorTop",&D3DXVECTOR4((float)Timecycle->m_nCurrentSkyTopRed/255.0f,
													(float)Timecycle->m_nCurrentSkyTopGreen/255.0f,
													(float)Timecycle->m_nCurrentSkyTopBlue/255.0f,
													1.0f));
	m_pEffect->SetVector("skyColorBottom",&D3DXVECTOR4((float)Timecycle->m_nCurrentSkyBottomRed/255.0f,
													(float)Timecycle->m_nCurrentSkyBottomGreen/255.0f,
													(float)Timecycle->m_nCurrentSkyBottomBlue/255.0f,
													1.0f));
	m_pEffect->SetVector("TopCloudColor",&D3DXVECTOR4((float)Timecycle->m_nCurrentFluffyCloudsBottomRed/255.0f,
													  (float)Timecycle->m_nCurrentFluffyCloudsBottomGreen/255.0f,
													  (float)Timecycle->m_nCurrentFluffyCloudsBottomBlue/255.0f,
													  1.0f));
	m_pEffect->SetFloat("fCloudCover",Timecycle->m_fAlpha1/255.0f);
	m_pEffect->SetFloat("fCloud1Transp",Timecycle->m_fCloudAlpha1/255.0f);
	m_pEffect->SetMatrix("gmWorldViewProj",&gm_WorldViewProjection);
	m_pEffect->SetMatrix("gmWorld",&gm_World);
	GetCurrentStates();
	m_pEffect->Begin(&passes,0);
	m_pEffect->BeginPass(0);
	m_pEffect->CommitChanges();
	if(skySphere){
		skySphere->DrawSubset(0);
	}
	m_pEffect->EndPass();
	m_pEffect->End();
	SetOldStates();
}
void CSkyRender::Release()
{
	if(skySphere){
		SAFE_RELEASE(skySphere);
	}
}