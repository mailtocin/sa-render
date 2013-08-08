#include "CDeferredRendering.h"
#include "CRender.h"
#include "CPatch.h"
#include "CGame.h"
#include "CDebug.h"
#include "CVehicleRender.h"
#include "CObjectRender.h"
#include "CPedsRender.h"
#include <stdio.h>
#include <sstream>
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}
// Shadows textures/surfaces
IDirect3DTexture9 *CDeferredRendering::shadow[4];
IDirect3DSurface9 *CDeferredRendering::shadowSurface[4];
// Geometry Buffer textures/surfaces
IDirect3DTexture9 *CDeferredRendering::gbuffer[3];
IDirect3DSurface9 *CDeferredRendering::gbSurface[3];
// Temporary Post-Processing textures/surfaces
IDirect3DTexture9 **CDeferredRendering::rtTmpSurface;
IDirect3DSurface9 **CDeferredRendering::rsTmpSurface;
// Lighting Buffer textures/surfaces
IDirect3DTexture9 *CDeferredRendering::lightingTexture;
IDirect3DSurface9 *CDeferredRendering::lightingSurface;
// Day/Night state
float* _daylightLightingState = (float*)0x8D12C0;
// Post-Process textures/surfaces count
int CDeferredRendering::ppTCcount;
// Post-Process textures/surfaces width/height
int *CDeferredRendering::surfWidth;
int *CDeferredRendering::surfHeight;
// Shadow View/Projection Matrix
D3DXMATRIX CDeferredRendering::g_mLightView[2];
D3DXMATRIX CDeferredRendering::g_mLightProj;
D3DXMATRIX CDeferredRendering::g_mLightProj2;
// Deferred/Post-Process Shader
ID3DXEffect *CDeferredRendering::m_pEffect;
// Noise texture for Post-Process
IDirect3DTexture9 *noise;
// Cloud texture for Sky
IDirect3DTexture9 *clouds;
// Cubemap for Reflections
IDirect3DCubeTexture9 *CDeferredRendering::cubemap;
// D3D parameters
D3DPRESENT_PARAMETERS *g_D3Dpp = (D3DPRESENT_PARAMETERS *) 0xC9C040;
// Shadow stuff
float m_fExtraDistance;
D3DXVECTOR3 *m_vUpVector;
D3DXVECTOR3 m_vLightDirection;

// Setup function
bool CDeferredRendering::Setup()
{
	ID3DXBuffer *errors;
	HRESULT result;
	m_vUpVector = new D3DXVECTOR3(0, 1, 0);
	// Loading shader... TODO: Load it from folder...
	result = D3DXCreateEffectFromFile(g_Device,"Deferred.fx", 0, 0, 0, 0, &m_pEffect, &errors);
	if(!CDebug::CheckForD3D9Errors(errors,"CVehicleRender::Setup: D3DXCreateEffectFromFile() - failed while compiling vechicle.fx",result))	{
		return false;
	}

	//------------Loading textures------------------------------------------
	D3DXCreateTextureFromFile(g_Device,"noise.png",&noise);
	D3DXCreateTextureFromFile(g_Device,"clouds.tga",&clouds);
	D3DXCreateCubeTextureFromFile(g_Device,"grace_diffuse_cube.dds",&cubemap);
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	//------------------------Post-Process stuff-----------------------------
	// 1) We need to get postprocessing techniques count.
	m_pEffect->GetInt("PostProcessCount",&ppTCcount);
	// 2) We need to allocate surface/texture arrays.
	rtTmpSurface = (IDirect3DTexture9**)calloc(ppTCcount,sizeof(IDirect3DTexture9*));
	rsTmpSurface = (IDirect3DSurface9**)calloc(ppTCcount,sizeof(IDirect3DSurface9*));
	surfWidth = (int*)calloc(ppTCcount,sizeof(int));
	surfHeight = (int*)calloc(ppTCcount,sizeof(int));
	// 3) We need to set sizes of textures
	if(ppTCcount>1){
		for(int i = 0; i<ppTCcount;i++){
			std::stringstream sstm1;
			sstm1 << "screenBuffer_" << i;
			std::string result1 = sstm1.str();
			m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),0),&surfWidth[i]);
			m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),1),&surfHeight[i]);
		}
	}
	std::stringstream sstm1;
	sstm1 << "screenBuffer_" << 0;
	std::string result1 = sstm1.str();
	m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),0),&surfWidth[0]);
	m_pEffect->GetInt(m_pEffect->GetAnnotation((char*)result1.c_str(),1),&surfHeight[0]);
	//----------------------------------------------------------------------
	return true;
}

// Light Struct... TODO: Move it to RenderWare.h
struct CLight{
	RwV3D pos, dir;
	float radius, red, green, blue;
	BYTE type; /*
			   0  Omni/Point   Using position and radius.
			   1  Directional  Using position, radius and direction.
			   2  Shadow       It's black!
			   4  Global       Using only position.
			   */
	BYTE subType; /*
				  0 Default.
				  1 Car headlights.
				  2 Car backlights.
				  3 Car LED 
				  */
	BYTE fogType, generateShadows;
};
//-------------------------------------Light stuff------------------------------------------------
int g_iNumNewLights;
CLight g_aNewLights[50];
DWORD g_aAddLightCallTable[] = { 0x47B0D8, 0x48ED76, 0x49DF47, 0x53632D,
	0x5364B4, 0x53AEAC, 0x6AB80F, 0x6ABBA6,
	0x6BD641, 0x6D4D14, 0x6E27E6, 0x6E28E7,
	0x6FD105, 0x6FD347, 0x737849, 0x7378C1,
	0x73AF74, 0x73CCFD, 0x740D68 };
void _AddLight(char type, float x, float y, float z, float x_dir, float y_dir, float z_dir,
			   float radius, float red, float green, float blue, char fogType, char generateExtraShadows,
			   int entityAffected)
{
	if(g_iNumNewLights < 50)
	{
		g_aNewLights[g_iNumNewLights].pos.x = x;
		g_aNewLights[g_iNumNewLights].pos.y = y;
		g_aNewLights[g_iNumNewLights].pos.z = z;
		g_aNewLights[g_iNumNewLights].dir.x = x_dir;
		g_aNewLights[g_iNumNewLights].dir.y = y_dir;
		g_aNewLights[g_iNumNewLights].dir.z = z_dir;
		g_aNewLights[g_iNumNewLights].radius = radius;
		g_aNewLights[g_iNumNewLights].red = red;
		g_aNewLights[g_iNumNewLights].green = green;
		g_aNewLights[g_iNumNewLights].blue = blue;
		g_aNewLights[g_iNumNewLights].type = type;
		g_aNewLights[g_iNumNewLights].generateShadows = generateExtraShadows;
		if(fogType > 100)
		{
			g_aNewLights[g_iNumNewLights].fogType = 0;
			g_aNewLights[g_iNumNewLights].subType = fogType - 100;
		}
		else
		{
			g_aNewLights[g_iNumNewLights].fogType = fogType;
			g_aNewLights[g_iNumNewLights].subType = 0;
		}
		g_iNumNewLights++;
	}
}
//------------------------------------------------------------------------------------------------
// Im3d Draw Function
HRESULT __fastcall Im3D_DrawIndexedPrimitive(int ecx0,
											 int edx0,
											 IDirect3DDevice9 *device,
											 D3DPRIMITIVETYPE Type,
											 INT BaseVertexIndex,
											 UINT MinIndex,
											 UINT NumVertices,
											 UINT StartIndex,
											 UINT PrimitiveCount)
{
	return TRUE;
}
// On Reset Device
void CDeferredRendering::Reset()
{
	if(m_pEffect)
		m_pEffect->OnResetDevice();
}
// Vertex Declaration for Full-Screen Quad
IDirect3DVertexDeclaration9*  VertDecl = NULL;

// Full-Screen Quad Drawing Function... TODO: Move it to somewhere...
void DrawFullScreenQuad(IDirect3DDevice9* pd3dDevice) {
	struct Vertex {
		D3DXVECTOR2 pos;
		D3DXVECTOR2 tex_coord;
	}quad[4];

	quad[0].pos = D3DXVECTOR2(-1,-1); quad[0].tex_coord = D3DXVECTOR2(0,1);
	quad[1].pos = D3DXVECTOR2(-1,1);  quad[1].tex_coord = D3DXVECTOR2(0,0);
	quad[2].pos = D3DXVECTOR2(1,-1);  quad[2].tex_coord = D3DXVECTOR2(1,1);
	quad[3].pos = D3DXVECTOR2(1,1);   quad[3].tex_coord = D3DXVECTOR2(1,0);

	const D3DVERTEXELEMENT9 Decl[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	pd3dDevice->CreateVertexDeclaration( Decl, &VertDecl );

	//rwD3D9SetVertexDeclaration( VertDecl );
	pd3dDevice->SetVertexDeclaration(VertDecl);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,rwCULLMODECULLNONE);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex));

	SAFE_RELEASE(VertDecl);
}

// Post-Process Drawing Pass.... TODO: idk what to do with it... maybe make passes count in value?
void CDeferredRendering::DrawPostProcessPass() {
	UINT pPasses;
	m_pEffect->Begin(&pPasses,0);
	m_pEffect->BeginPass(0);
	DrawFullScreenQuad(g_Device);
	m_pEffect->EndPass();
	m_pEffect->End();
}

// On Lost Device
void CDeferredRendering::Lost()
{
	if (shadowSurface[0])
		shadowSurface[0]->Release();
	if (shadow[0])
		shadow[0]->Release();
	shadow[0] = NULL;
	if (shadowSurface[1])
		shadowSurface[1]->Release();
	if (shadow[1])
		shadow[1]->Release();
	shadow[1] = NULL;
	if (shadowSurface[2])
		shadowSurface[2]->Release();
	if (shadow[2])
		shadow[2]->Release();
	shadow[2] = NULL;
	if (shadowSurface[3])
		shadowSurface[3]->Release();
	if (shadow[3])
		shadow[3]->Release();
	shadow[3] = NULL;
	if (gbSurface[0])
		gbSurface[0]->Release();
	if (gbuffer[0])
		gbuffer[0]->Release();
	gbuffer[0] = NULL;
	if (gbSurface[1])
		gbSurface[1]->Release();
	if (gbuffer[1])
		gbuffer[1]->Release();
	gbuffer[1] = NULL;
	if (gbSurface[2])
		gbSurface[2]->Release();
	if (gbuffer[2])
		gbuffer[2]->Release();
	gbuffer[2] = NULL;
	if (lightingSurface)
		lightingSurface->Release();
	if (lightingTexture)
		lightingTexture->Release();
	lightingTexture = NULL;
	//if (cubemap)
	//   cubemap->Release();
	//cubemap = NULL;
	for(int i = 0; i<ppTCcount;i++){
		if (rsTmpSurface[i])
			rsTmpSurface[i]->Release();
		if (rtTmpSurface[i])
			rtTmpSurface[i]->Release();
		rtTmpSurface[i] = NULL;
	}
	if(m_pEffect)
		m_pEffect->OnLostDevice();
}

// Patch Function
void CDeferredRendering::Patch()
{
	CPatch::RedirectCall(0x53ECBD, Idle);
	for(int i = 0; i<19; i++)
		CPatch::RedirectCall(g_aAddLightCallTable[i], _AddLight);
	CPatch::Nop(0x80E707, 1);
	CPatch::RedirectCall(0x80E708, Im3D_DrawIndexedPrimitive);
	CPatch::Nop(0x80E931, 1);
	CPatch::RedirectCall(0x80E932, Im3D_DrawIndexedPrimitive);
}

// Post-Process loop
void CDeferredRendering::PostProcess(IDirect3DSurface9 *outSurf){
	if(ppTCcount>1){
		for(int i = 0; i<ppTCcount-1;i++){
			g_Device->SetRenderTarget(0,rsTmpSurface[i+1]);
			std::stringstream sstm;
			sstm << "PostProcess_" << i;
			std::string result = sstm.str();
			m_pEffect->SetTechnique((char*)result.c_str());

			std::stringstream sstm1;
			sstm1 << "screenBuffer_" << i;
			std::string result1 = sstm1.str();
			m_pEffect->SetTexture((char*)result1.c_str(),rtTmpSurface[i]);
			DrawPostProcessPass();
		}
	}
	g_Device->SetRenderTarget(0,outSurf);
	std::stringstream sstm;
	sstm << "PostProcess_" << ppTCcount-1;
	std::string result = sstm.str();
	m_pEffect->SetTechnique((char*)result.c_str());
	std::stringstream sstm1;
	sstm1 << "screenBuffer_" << ppTCcount-1;

	std::string result1 = sstm1.str();
	m_pEffect->SetTexture((char*)result1.c_str(),rtTmpSurface[ppTCcount-1]);
	DrawPostProcessPass();
}

// Cubemap rendering.. TODO: Make it WORK!!!!1111
void CDeferredRendering::DrawCubemap(){
	RwCameraEndUpdate(Scene->m_pRwCamera);
	if(!cubemap){
		g_Device->CreateCubeTexture(64,1,D3DUSAGE_RENDERTARGET,D3DFMT_R8G8B8,D3DPOOL_DEFAULT,&cubemap,NULL);
	}
	IDirect3DSurface9* pOldRTSurf= NULL,*m_pZBuffer = NULL;
	D3DXMATRIX tmpview;
	IDirect3DSurface9* tmpsurf;
	HRESULT hr;
	D3DXMATRIX view,proj;
	g_Device->GetTransform(D3DTS_VIEW,&view);
	g_Device->GetTransform(D3DTS_PROJECTION, &proj);
	g_Device->GetRenderTarget(0, &pOldRTSurf);
	g_Device->GetDepthStencilSurface(&m_pZBuffer);
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/2, 1.0f, 0.5f, 1000.0f);
	CVector camPos;
	FindPlayerCoors(&camPos, 0);
	for(DWORD i=0; i<6; i++)
	{
		// Standard view that will be overridden below
		D3DXVECTOR3 vEnvEyePt = D3DXVECTOR3(camPos.x, camPos.y, camPos.z);
		D3DXVECTOR3 vLookatPt, vUpVec;

		switch(i)
		{
		case D3DCUBEMAP_FACE_POSITIVE_X:
			vLookatPt = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			vLookatPt = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			vLookatPt = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			vLookatPt = D3DXVECTOR3(0.0f,-1.0f, 0.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
			vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			vLookatPt = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
			vUpVec    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		}
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH(&matView, &vEnvEyePt, &(vLookatPt+vEnvEyePt), &vUpVec);
		IDirect3DSurface9 *pFace;
		cubemap->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pFace);
		hr = g_Device->BeginScene();
		g_Device->SetRenderTarget(0,pFace);
		SAFE_RELEASE(pFace);
		g_Device->SetDepthStencilSurface(m_pZBuffer);
		if (hr == S_OK) g_Device->EndScene();
		RwCameraBeginUpdate(Scene->m_pRwCamera);
		RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
		g_Device->SetTransform(D3DTS_VIEW,&matView);
		g_Device->SetTransform(D3DTS_PROJECTION,&matProj);
		CObjectRender::m_pEffect->SetTechnique("Shadow");
		CVehicleRender::m_pEffect->SetTechnique("Shadow");
		CPedsRender::m_pEffect->SetTechnique("Shadow");
		_RenderScene();
		RenderPedWeapons();
		RwCameraEndUpdate(Scene->m_pRwCamera);
	}
	hr = g_Device->BeginScene();
	g_Device->SetRenderTarget(0,pOldRTSurf);
	g_Device->SetDepthStencilSurface(m_pZBuffer);
	SAFE_RELEASE(pOldRTSurf);
	SAFE_RELEASE(m_pZBuffer);
	if (hr == S_OK) g_Device->EndScene();
	RwCameraBeginUpdate(Scene->m_pRwCamera);
	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	g_Device->SetTransform(D3DTS_VIEW,&view);
	g_Device->SetTransform(D3DTS_PROJECTION,&proj);
}

// Shadow Mapping Function
void CDeferredRendering::ComputeShadowMap(IDirect3DSurface9*shadowSurface,IDirect3DSurface9*shadowSurfaceC,float distance,D3DXMATRIX*lightview,D3DXMATRIX*lightproj,float dd)
{
	RwCameraEndUpdate(Scene->m_pRwCamera);
	RwV3D sunpos;
	D3DXVECTOR4 cam;
	GetSunPosn(&sunpos,distance);
	cam = *(D3DXVECTOR4*)GetCamPos();
	m_vLightDirection.x = sunpos.x;
	m_vLightDirection.y = sunpos.y;
	m_vLightDirection.z = sunpos.z;
	float fDistance;
	float faNear[3];
	float faFar[3];
	float faDiff[3]; // 0 = xaxis, 1 = yaxis, 2 = zaxis
	D3DXVECTOR3 vaAxis[3];
	D3DXVECTOR3 vCenterPosition;
	D3DXVECTOR3 tmp;
	int i, k;
	for (i = 0; i < 3; i++)
	{
		faNear[i] = FLT_MAX;
		faFar[i] = FLT_MIN;
	}
	D3DXVec3Normalize(&vaAxis[2],&m_vLightDirection);
	D3DXVec3Cross(&tmp,m_vUpVector, &vaAxis[2]);
	D3DXVec3Normalize(&vaAxis[0],&tmp);
	D3DXVec3Cross(&tmp,&vaAxis[2], &vaAxis[0]);
	D3DXVec3Normalize(&vaAxis[1],&tmp);
	for (i = 0; i < 8; i++)
	{
		for (k = 0; k < 3; k++)
		{
			fDistance = D3DXVec3Dot(&vaAxis[k], new D3DXVECTOR3( Scene->m_pRwCamera->frustumCorners[i].x,Scene->m_pRwCamera->frustumCorners[i].y,Scene->m_pRwCamera->frustumCorners[i].z));

			if (fDistance < faNear[k])
				faNear[k] = fDistance;

			if (fDistance > faFar[k])
				faFar[k] = fDistance;
		}
	}
	for (i = 0; i < 3; i++)
	{
		faDiff[i] = faFar[i] - faNear[i];
		vCenterPosition += vaAxis[i] * (faNear[i] + faFar[i]);
	}
	vCenterPosition *= 0.5f;
	float fExtraDistance = m_fExtraDistance;
	float ZNear = 50.0f;
	float fLightZFar = faDiff[2] + ZNear + fExtraDistance;
	D3DXVECTOR3 vLightPosition = vCenterPosition - vaAxis[2] * (faDiff[2] * 0.5f + ZNear + fExtraDistance);

	IDirect3DSurface9* pOldRTSurf= NULL,*m_pZBuffer = NULL;

	CVector camPos;
	HRESULT hr;
	D3DXMATRIX view,proj;

	g_Device->GetTransform(D3DTS_VIEW,&view);
	g_Device->GetTransform(D3DTS_PROJECTION,&proj);
	FindPlayerCoors(&camPos, 0);
	//D3DXMatrixLookAtLH(lightview,&vLightPosition,&vCenterPosition,new D3DXVECTOR3(0,1,0));
	D3DXMatrixLookAtLH(lightview,new D3DXVECTOR3(camPos.x+(sunpos.x),camPos.y+(sunpos.y),camPos.z+(sunpos.z)),
		new D3DXVECTOR3(camPos.x,camPos.y,camPos.z),new D3DXVECTOR3(0,1,0));
	D3DXMatrixOrthoLH(lightproj,faDiff[0]/dd, faDiff[1]/dd, -ZNear, fLightZFar);
	//D3DXMatrixPerspectiveFovLH(&g_mLightProj, D3DXToRadian(60.0f), 1.0f, 0.3f, 1500.0f);
	g_Device->GetRenderTarget(0, &pOldRTSurf);
	g_Device->GetDepthStencilSurface(&m_pZBuffer);
	pOldRTSurf->Release();
	m_pZBuffer->Release();
	hr = g_Device->BeginScene();
	g_Device->SetRenderTarget(0,shadowSurfaceC);
	g_Device->SetDepthStencilSurface(shadowSurface);
	if (hr == S_OK) g_Device->EndScene();
	RwCameraBeginUpdate(Scene->m_pRwCamera);

	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	CVehicleRender::m_pEffect->SetTechnique("Shadow");
	CObjectRender::m_pEffect->SetTechnique("Shadow");
	CPedsRender::m_pEffect->SetTechnique("Shadow");
	g_Device->SetTransform(D3DTS_VIEW,lightview);
	g_Device->SetTransform(D3DTS_PROJECTION,lightproj);
	//g_Device->SetViewport(&newViewport);
	_RenderScene();
	RenderPedWeapons();
	RwCameraEndUpdate(Scene->m_pRwCamera);
	hr = g_Device->BeginScene();
	g_Device->SetRenderTarget(0,pOldRTSurf);
	g_Device->SetDepthStencilSurface(m_pZBuffer);
	pOldRTSurf->Release();
	m_pZBuffer->Release();
	if (hr == S_OK) g_Device->EndScene();
	RwCameraBeginUpdate(Scene->m_pRwCamera);
	RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
	g_Device->SetTransform(D3DTS_VIEW,&view);
	g_Device->SetTransform(D3DTS_PROJECTION,&proj);
}

//------------------------------------------SkySphere Stuff------------------------------------------
struct _VERTEX {
	D3DXVECTOR3 pos;     // vertex position
	D3DXVECTOR3 norm;    // vertex normal
	float tu;            // texture coordinates
	float tv;
} VERTEX,*LPVERTEX;

#define FVF_VERTEX    D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1
LPD3DXMESH CreateMappedSphere(LPDIRECT3DDEVICE9 pDev,float fRad,UINT slices,UINT stacks) {
	// create the sphere
	LPD3DXMESH mesh;
	if (FAILED(D3DXCreateSphere(pDev,fRad,slices,stacks,&mesh,NULL)))
		return NULL;

	// create a copy of the mesh with texture coordinates,
	// since the D3DX function doesn't include them
	LPD3DXMESH texMesh;
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM,FVF_VERTEX,pDev,&texMesh)))
		// failed, return un-textured mesh
			return mesh;

	// finished with the original mesh, release it
	mesh->Release();

	// lock the vertex buffer
	_VERTEX* pVerts = NULL;
	if (SUCCEEDED(texMesh->LockVertexBuffer(0,(VOID **) &pVerts))) {

		// get vertex count
		int numVerts=texMesh->GetNumVertices();

		// loop through the vertices
		for (int i=0;i<numVerts;i++) {

			// calculate texture coordinates
			pVerts->tu=asinf(pVerts->norm.x)/D3DX_PI+0.5f;
			pVerts->tv=asinf(pVerts->norm.y)/D3DX_PI+0.5f;

			// go to next vertex
			pVerts++;
		}

		// unlock the vertex buffer
		texMesh->UnlockVertexBuffer();
	}

	// return pointer to caller
	return texMesh;
}
//---------------------------------------------------------------------------------------------------

// Deferred Lighting Optimizations
RECT DetermineClipRect(const D3DXVECTOR3& position, const float range,D3DXMATRIX m_View,D3DXMATRIX m_Projection,float screenW,float screenH)
{
	//compute 3D bounding box of light in world space
	D3DXVECTOR3 bbox3D[8];
	bbox3D[0].x = position.x - range;  bbox3D[0].y = position.y + range;  bbox3D[0].z = position.z - range;
	bbox3D[1].x = position.x + range;  bbox3D[1].y = position.y + range;  bbox3D[1].z = position.z - range;
	bbox3D[2].x = position.x - range;  bbox3D[2].y = position.y - range;  bbox3D[2].z = position.z - range;
	bbox3D[3].x = position.x + range;  bbox3D[3].y = position.y - range;  bbox3D[3].z = position.z - range;
	bbox3D[4].x = position.x - range;  bbox3D[4].y = position.y + range;  bbox3D[4].z = position.z + range;
	bbox3D[5].x = position.x + range;  bbox3D[5].y = position.y + range;  bbox3D[5].z = position.z + range;
	bbox3D[6].x = position.x - range;  bbox3D[6].y = position.y - range;  bbox3D[6].z = position.z + range;
	bbox3D[7].x = position.x + range;  bbox3D[7].y = position.y - range;  bbox3D[7].z = position.z + range;

	//project coordinates
	D3DXMATRIX viewProjMat = m_View * m_Projection;
	D3DXVECTOR2 projBox[8];
	for (int i = 0; i < 8; ++i)
	{
		D3DXVECTOR4 projPoint;
		D3DXVec3Transform(&projPoint, &bbox3D[i], &viewProjMat);
		projBox[i].x = projPoint.x / projPoint.w;  
		projBox[i].y = projPoint.y / projPoint.w;

		//clip to extents
		if (projBox[i].x < -1.0f)
			projBox[i].x = -1.0f;
		else if (projBox[i].x > 1.0f)
			projBox[i].x = 1.0f;
		if (projBox[i].y < -1.0f)
			projBox[i].y = -1.0f;
		else if (projBox[i].y > 1.0f)
			projBox[i].y = 1.0f;

		//go to pixel coordinates
		projBox[i].x = ((projBox[i].x + 1.0f) / 2.0f) * screenW;
		projBox[i].y = ((-projBox[i].y + 1.0f) / 2.0f) * screenH;
	}

	//compute 2D bounding box of projected coordinates
	unsigned int minX = 0xFFFFFFFF;
	unsigned int maxX = 0x00000000;
	unsigned int minY = 0xFFFFFFFF;
	unsigned int maxY = 0x00000000;
	for (int i = 0; i < 8; ++i)
	{
		unsigned int x = static_cast<unsigned int>(projBox[i].x);
		unsigned int y = static_cast<unsigned int>(projBox[i].y);
		if (x < minX)
			minX = x;
		if (x > maxX)
			maxX = x;
		if (y < minY)
			minY = y;
		if (y > maxY)
			maxY = y;
	}
	RECT bbox2D;
	bbox2D.top    = minY;
	bbox2D.bottom = maxY;
	bbox2D.left   = minX;
	bbox2D.right  = maxX;

	return bbox2D;
}

//Idle function
void CDeferredRendering::Idle(void *a)
{
	unsigned int time;
	HRESULT hr;
	RwV2d mousePos;
	D3DXVECTOR4 sun,cam;
	float angle;
	CVector axis_X, axis_Y;
	RwMatrix ols;
	do 
	time = GetTimeFromRenderStart();
	while(time / GetTimerDivider() - LastTickTime < 14);
	LastTickTime = GetTimeFromRenderStart() / GetTimerDivider();
	UpdateTimer();           // CTimer::Update
	InitSprite2dPerFrame();  // CSprite2d::InitPerFrame
	NumPointLights = 0;      // CPointLights::NumLights
	g_iNumNewLights = 0;
	InitFontPerFrame();      // CFont::InitPerFrame
	ProcessGame();           // CGame::Process
	ServiceDMAudio(DMAudio); // cDMAudio::Service
	SetLightsWithTimeOfDayColour(Scene->m_pRpWorld);
	if(!a)
		return;
	if(FrontEndMenuManager->m_bMenuActive || GetCameraScreenFadeStatus(TheCamera) == 2)
	{
		CalculateAspectRatio(); // CDraw::CalculateAspectRatio
		CameraSize(Scene->m_pRwCamera, 0, tan(gfFOV * 0.0087266462), gfAspectRatio);
		SetRenderWareCamera(Scene->m_pRwCamera); // CVisibilityPlugins::SetRenderWareCamera
		RwCameraClear(Scene->m_pRwCamera, gColourTop, 2);
		if(!RsCameraBeginUpdate(Scene->m_pRwCamera))
			return;
	}
	else
	{
		mousePos.x = RsGlobal->MaximumWidth * 0.5;
		mousePos.y = RsGlobal->MaximumHeight * 0.5;
		RsMouseSetPos(&mousePos);
		ConstructRendererRenderList(); // CRenderer::ConstructRenderList
		PreRenderRenderer();           // CRenderer::PreRender
		ProcessPedTasks();
		CreateShadowManagerShadows(ShadowManager); // CShadowManager::CreateShadows
		//RenderSceneGeometryToMirror();
		if(LightningFlash) // CWeather::LightningFlash
		{
			Timecycle->m_nCurrentSkyBottomRed = 255;
			Timecycle->m_nCurrentSkyBottomGreen = 255;
			Timecycle->m_nCurrentSkyBottomBlue = 255;
			if(!DoRWStuffStartOfFrame_Horizon(255, 255, 255, 255, 255, 255, 255))
				return;
		}
		else if(!DoRWStuffStartOfFrame_Horizon(Timecycle->m_nCurrentSkyTopRed, Timecycle->m_nCurrentSkyTopGreen, Timecycle->m_nCurrentSkyTopBlue,
			Timecycle->m_nCurrentSkyBottomRed, Timecycle->m_nCurrentSkyBottomGreen, Timecycle->m_nCurrentSkyBottomBlue, 255))
			return;
		DefinedState();
		RwCameraSetFarClipPlane(Scene->m_pRwCamera, 1500);
		Scene->m_pRwCamera->fogPlane = Timecycle->m_fCurrentFogStart;
		//RenderMirrors();
		RwCameraEndUpdate(Scene->m_pRwCamera);
		CVehicleRender::m_pEffect->SetFloat("screenHeight",RsGlobal->MaximumHeight);
		CVehicleRender::m_pEffect->SetFloat("screenWidth",RsGlobal->MaximumWidth);
		CObjectRender::m_pEffect->SetFloat("screenHeight",RsGlobal->MaximumHeight);
		CObjectRender::m_pEffect->SetFloat("screenWidth",RsGlobal->MaximumWidth);
		CPedsRender::m_pEffect->SetFloat("screenHeight",RsGlobal->MaximumHeight);
		CPedsRender::m_pEffect->SetFloat("screenWidth",RsGlobal->MaximumWidth);

		if(!shadow[0]){
			g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&shadow[0],NULL);
			shadow[0]->GetSurfaceLevel(0,&shadowSurface[0]);
		}
		if(!shadow[1]){
			g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_DEPTHSTENCIL,D3DFMT_D24S8,D3DPOOL_DEFAULT,&shadow[1],NULL);
			shadow[1]->GetSurfaceLevel(0,&shadowSurface[1]);
		}
		if(!shadow[2]){
			g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&shadow[2],NULL);
			shadow[2]->GetSurfaceLevel(0,&shadowSurface[2]);
		}
		if(!shadow[3]){
			g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_DEPTHSTENCIL,D3DFMT_D24S8,D3DPOOL_DEFAULT,&shadow[3],NULL);
			shadow[3]->GetSurfaceLevel(0,&shadowSurface[3]);
		}
		for(int i =0;i<3;i++){
			if(!gbuffer[i]){
				g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,D3DFMT_A16B16G16R16F,D3DPOOL_DEFAULT,&gbuffer[i],NULL);
				gbuffer[i]->GetSurfaceLevel(0,&gbSurface[i]);
			}
		}
		if(!lightingTexture){
			g_Device->CreateTexture(RsGlobal->MaximumWidth,RsGlobal->MaximumHeight,0,D3DUSAGE_RENDERTARGET,D3DFMT_A16B16G16R16F,D3DPOOL_DEFAULT,&lightingTexture,NULL);
			lightingTexture->GetSurfaceLevel(0,&lightingSurface);
		}
		if(ppTCcount>0){
			for(int i = 0; i<ppTCcount;i++){
				if(!rtTmpSurface[i]){
					g_Device->CreateTexture(surfWidth[i],surfHeight[i],0,D3DUSAGE_RENDERTARGET,D3DFMT_A16B16G16R16F,D3DPOOL_DEFAULT,&rtTmpSurface[i],NULL);
					rtTmpSurface[i]->GetSurfaceLevel(0,&rsTmpSurface[i]);
				}
			}
		}
		IDirect3DSurface9* pOldRTSurf= NULL,*m_pZBuffer = NULL;
		LPD3DXMESH sphere = CreateMappedSphere(g_Device,1000,100,100);
		D3DXMATRIX meshMat, meshRotate, meshTranslate,tmp,meshMatinv,view,proj,viewproj,invview,invproj,invviewproj;
		RwV3D sunpos,camPos;

		GetSunPosn(&sunpos,1000);
		D3DVIEWPORT9 vp;

		FindPlayerCoors(&camPos, 0);
		RwCameraBeginUpdate(Scene->m_pRwCamera);
		//ComputeShadowMap(shadowSurface[1],shadowSurface[0],150,&g_mLightView[0],&g_mLightProj,45);
		//ComputeShadowMap(shadowSurface[3],shadowSurface[2],150,&g_mLightView[1],&g_mLightProj2,5);
		//DrawCubemap();
		RwCameraEndUpdate(Scene->m_pRwCamera);
		hr = g_Device->BeginScene();
		g_Device->GetRenderTarget(0, &pOldRTSurf);
		pOldRTSurf->Release();
		g_Device->SetRenderTarget(0,gbSurface[0]);
		g_Device->SetRenderTarget(1,gbSurface[1]);
		g_Device->SetRenderTarget(2,gbSurface[2]);
		if (hr == S_OK) g_Device->EndScene();
		g_Device->GetTransform(D3DTS_VIEW,&view);
		D3DXMatrixInverse(&invview,NULL,&view);
		g_Device->GetTransform(D3DTS_PROJECTION,&proj);
		D3DXMatrixInverse(&invproj,NULL,&proj);
		D3DXMatrixMultiply(&viewproj,&view,&proj);
		D3DXMatrixInverse(&invviewproj,NULL,&viewproj);

		D3DXMatrixIdentity(&meshMat);
		D3DXMatrixRotationY(&meshRotate, D3DXToRadian(0));
		D3DXMatrixTranslation(&meshTranslate, camPos.x,camPos.y,camPos.z);
		D3DXMatrixMultiply(&meshMat, &meshRotate, &meshTranslate);
		D3DXMatrixMultiplyTranspose(&tmp, &meshMat, &viewproj);
		RwCameraBeginUpdate(Scene->m_pRwCamera);
		RwCameraClear(Scene->m_pRwCamera, gColourTop, 3);
		CObjectRender::m_pEffect->SetTechnique("Deferred");
		CVehicleRender::m_pEffect->SetTechnique("Deferred");
		CPedsRender::m_pEffect->SetTechnique("Deferred");
		_RenderScene();
		RenderPedWeapons();
		UINT passes;
		D3DXCOLOR ambientColor,ambientColor2;
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

		m_pEffect->SetVector("lightDirection",new D3DXVECTOR4(camPos.x+(sunpos.x),camPos.y+(sunpos.y),camPos.z+(sunpos.z),1));
		m_pEffect->SetTexture("cloudTex",clouds);
		m_pEffect->SetVector("SunColor",new D3DXVECTOR4(Timecycle->m_nCurrentSunCoreRed/255.0f*(1-*_daylightLightingState),Timecycle->m_nCurrentSunCoreGreen/255.0f*(1-*_daylightLightingState),Timecycle->m_nCurrentSunCoreBlue/255.0f*(1-*_daylightLightingState),1));
		m_pEffect->SetTechnique("sky");
		m_pEffect->SetMatrix("gmWorldViewProj",&tmp);
		m_pEffect->SetMatrix("gmViewProj",&viewproj);
		m_pEffect->SetMatrix("gmProj",&proj);
		m_pEffect->SetMatrix("gmViewProjInv",&invviewproj);
		m_pEffect->SetMatrix("gmViewInv",&invview);
		m_pEffect->SetMatrix("gmView",&view);
		m_pEffect->SetMatrix("gmInvProj",&invproj);
		m_pEffect->SetMatrix("gmWorld",&meshMat);
		m_pEffect->SetTexture("test",shadow[1]);
		m_pEffect->SetTexture("test2",shadow[3]);
		m_pEffect->Begin(&passes,0);
		m_pEffect->BeginPass(0);
		sphere->DrawSubset(0);
		m_pEffect->EndPass();
		m_pEffect->End();
		RwCameraEndUpdate(Scene->m_pRwCamera);

		if (sphere) sphere->Release();
		DWORD dwOldFVF;
		DWORD dwSBlend;
		DWORD dwDBlend;
		DWORD dwBlendOp;
		DWORD dwCull;
		IDirect3DVertexBuffer9* pSSData;
		UINT dwSSOffset;
		UINT dwSSSize;
		const DWORD dwFVF_POST = D3DFVF_XYZRHW | D3DFVF_TEX1;
		IDirect3DBaseTexture9* pOldTexture;
		IDirect3DVertexDeclaration9* pOldVDecl;
		hr = g_Device->BeginScene();
		g_Device->GetTexture(0, &pOldTexture);
		//g_Device->GetVertexDeclaration(&pOldVDecl);
		g_Device->GetFVF(&dwOldFVF);
		g_Device->GetRenderState(D3DRS_SRCBLEND, &dwSBlend);
		g_Device->GetRenderState(D3DRS_DESTBLEND, &dwDBlend);
		g_Device->GetRenderState(D3DRS_BLENDOP, &dwBlendOp);
		g_Device->GetRenderState(D3DRS_CULLMODE, &dwCull);
		g_Device->GetStreamSource(0, &pSSData, &dwSSOffset, &dwSSSize);
		//g_Device->StretchRect((*(IDirect3DSurface9**)0xC98090), NULL, gbSurface[0], NULL, D3DTEXF_POINT);
		//pOldRTSurf->Release();
		UINT pss;
		g_Device->SetTexture(0, NULL);
		g_Device->SetFVF(dwFVF_POST);
		g_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		g_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		g_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		g_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pEffect->SetTechnique("DefShad");
		D3DXMATRIX m_LightViewProj[2];
		g_Device->SetRenderTarget(0,lightingSurface);
		g_Device->SetRenderTarget(1,NULL);
		g_Device->SetRenderTarget(2,NULL);
		D3DXMatrixMultiplyTranspose(&m_LightViewProj[0],&g_mLightView[0],&g_mLightProj);
		D3DXMatrixMultiplyTranspose(&m_LightViewProj[1],&g_mLightView[1],&g_mLightProj2);
		m_pEffect->SetMatrixArray("gmLightViewProj",m_LightViewProj,2);

		m_pEffect->SetTexture("colorBuffer",gbuffer[0]);
		m_pEffect->SetTexture("normalSpecBuffer",gbuffer[1]);
		m_pEffect->SetTexture("shadowDepthBuffer",gbuffer[2]);
		cam = *(D3DXVECTOR4*)GetCamPos();
		m_pEffect->SetTexture("noise",noise);
		m_pEffect->SetTexture("cubemap",cubemap);
		m_pEffect->SetVector("vDir",new D3DXVECTOR4(cam.x-camPos.x,cam.y-camPos.y,cam.z-camPos.z,1));
		m_pEffect->SetVector("sLP",new D3DXVECTOR4(sunpos.x+camPos.x,sunpos.y+camPos.y,sunpos.z+camPos.z,1));
		DrawPostProcessPass();
		m_pEffect->SetTechnique("DefShadPL");
		if(g_iNumNewLights>1) {
			for(int l = 0; l<g_iNumNewLights;l++){
				D3DXVECTOR3 pos;
				RECT sr;
				if(g_aNewLights[l].subType != 1){
					pos = D3DXVECTOR3(g_aNewLights[l].pos.x,g_aNewLights[l].pos.y,g_aNewLights[l].pos.z);
					sr = DetermineClipRect(pos,g_aNewLights[l].radius,view,proj,1024,768);
					m_pEffect->SetVector("sLP",new D3DXVECTOR4(g_aNewLights[l].pos.x,g_aNewLights[l].pos.y,g_aNewLights[l].pos.z,1));
					m_pEffect->SetVector("PointLightColor",new D3DXVECTOR4(g_aNewLights[l].red,g_aNewLights[l].green,g_aNewLights[l].blue,1));
					m_pEffect->SetFloat("PointLightRange",g_aNewLights[l].radius);
					g_Device->SetScissorRect(&sr);
					DrawPostProcessPass();
				}
			}
		}
		g_Device->SetRenderTarget(0,rsTmpSurface[0]);
		m_pEffect->SetTexture("lightBuffer",lightingTexture);
		m_pEffect->SetTechnique("DefShadL");
		DrawPostProcessPass();


		PostProcess(pOldRTSurf);
		//g_Device->SetVertexDeclaration(pOldVDecl);
		//if (pOldVDecl) pOldVDecl->Release();
		g_Device->SetFVF(dwOldFVF);
		g_Device->SetRenderState(D3DRS_SRCBLEND, dwSBlend);
		g_Device->SetRenderState(D3DRS_DESTBLEND, dwDBlend);
		g_Device->SetRenderState(D3DRS_BLENDOP, dwBlendOp);
		g_Device->SetRenderState(D3DRS_CULLMODE, dwCull);
		g_Device->SetStreamSource(0, pSSData, dwSSOffset, dwSSSize);
		g_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		if (pSSData) pSSData->Release();
		if (hr == S_OK) g_Device->EndScene();
		RwCameraBeginUpdate(Scene->m_pRwCamera);
		RenderEffects();
		sub_53E8D0(g_Unk);
		if((!TheCamera->m_BlurType || TheCamera->m_BlurType == 2) && TheCamera->m_ScreenReductionPercentage > 0.0 )
			SetCameraMotionBlurAlpha(TheCamera, 150); // CCamera::SetMotionBlurAlpha
		RenderCameraMotionBlur(TheCamera);            // CCamera::RenderMotionBlur
		Render2dStuff();
	}
	if(FrontEndMenuManager->m_bMenuActive)
		DrawMenuManagerFrontEnd(FrontEndMenuManager); // CMenuManager::DrawFrontEnd
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
	DoFade();
	DrawStyledText();
	DrawMessages(0); // CMessages::Display
	DrawFonts();     // CFont::DrawFonts
	if(CreditsOn)
	{
		if ( !FrontEndMenuManager->m_bMenuActive )
			RenderCredits();       // CCredits::Render
		if ( CreditsOn )
		{
			if ( !FrontEndMenuManager->m_bMenuActive )
				RenderCredits();   // CCredits::Render
		}
	}
	DebugDisplayTextBuffer(); // CDebug::DebugDisplayTextBuffer
	FlushObrsPrintfs();
	RwCameraEndUpdate(Scene->m_pRwCamera);
	RsCameraShowRaster(Scene->m_pRwCamera);
}