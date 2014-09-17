/********************************SA::Render source file*************************************/
/* File name   : CGame.cpp                                                                 */
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac, PetkaGtA                                                         */
/* File descrip: GTA structures/enumerations/functions/defines are represented here.       */
/* File created: 17.05.2013                                                                */
/* File last ed: unk                                                                       */
/*******************************************************************************************/
#include "..\Headers\CGame.h"
D3DXVECTOR4 CGlobalValues::gm_SunPosition;
DWORD CGlobalValues::gm_BlendStates[27];
bool GetSunPosn(CVector *dst)
{
	float sunDistance;
	CVector *sunPos, *camPos;
	bool res;
	sunDistance = *(float*)0xC3EF9C * 0.95f;
	sunPos = (CVector *)(*(DWORD *)0xB79FD0 * 12 + 0xB7CA50);
	if (sunPos->z > -0.1f)
		res = TRUE;
	else
		res = FALSE;
	if (*(DWORD*)0xB6F03C)
	{
		camPos = (CVector *)(0xB6F03C + 0x30);
		dst->x = sunPos->x * sunDistance + camPos->x;
		dst->y = sunPos->y * sunDistance + camPos->y;
		dst->z = sunPos->z * sunDistance + camPos->z;
	}
	else
	{
		camPos = (CVector *)0xB6F02C;
		dst->x = sunPos->x * sunDistance + camPos->x;
		dst->y = sunPos->y * sunDistance + camPos->y;
		dst->z = sunPos->z * sunDistance + camPos->z;
	}
	return res;
}

CBaseModelInfo** GetModelPtrs(){
	return *(CBaseModelInfo***)0x40122D;
}

bool GetSunPosn(CVector *dst, float farClip)
{
	CVector *sunPos, *camPos;
	bool res;
	sunPos = (CVector *)(*(DWORD *)0xB79FD0 * 12 + 0xB7CA50);
	if (sunPos->z > -0.1f)
		res = TRUE;
	else
		res = FALSE;
	if (*(DWORD*)0xB6F03C)
	{
		camPos = (CVector *)(0xB6F03C + 0x30);
		dst->x = sunPos->x * farClip + camPos->x;
		dst->y = sunPos->y * farClip + camPos->y;
		dst->z = sunPos->z * farClip + camPos->z;
	}
	else
	{
		camPos = (CVector *)0xB6F02C;
		dst->x = sunPos->x * farClip + camPos->x;
		dst->y = sunPos->y * farClip + camPos->y;
		dst->z = sunPos->z * farClip + camPos->z;
	}
	dst->x *= (1-(*_daylightLightingState))?1:-1;
	dst->y *= ((1 - (*_daylightLightingState))) ? 1 : -1;
	dst->z *= (1 - (*_daylightLightingState)) ? 1 : -1;
	return res;
}

CVector *GetCamPos()
{
	return &TheCamera->m_vGameCamPos;
}

bool Im2DRenderQuad(float x1, float y1, float x2, float y2, float z, float recipCamZ, float uvOffset, unsigned int color)
{
	float uvOffsetB;
	RwD3D9Vertex vertices[4];

	uvOffsetB = uvOffset + 1.0f;
	vertices[0].x = x1;
	vertices[0].y = y1;
	vertices[0].z = z;
	vertices[0].rhw = recipCamZ;
	vertices[0].u = uvOffset;
	vertices[0].v = uvOffset;
	vertices[1].x = x1;
	vertices[1].y = y2;
	vertices[1].z = z;
	vertices[1].rhw = recipCamZ;
	vertices[1].u = uvOffset;
	vertices[1].v = uvOffsetB;
	vertices[2].x = x2;
	vertices[2].y = y1;
	vertices[2].z = z;
	vertices[2].rhw = recipCamZ;
	vertices[2].u = uvOffsetB;
	vertices[2].v = uvOffset;
	vertices[3].x = x2;
	vertices[3].y = y2;
	vertices[3].z = z;
	vertices[3].rhw = recipCamZ;
	vertices[3].u = uvOffsetB;
	vertices[3].v = uvOffsetB;
	vertices[0].emissiveColor = color;
	vertices[1].emissiveColor = color;
	vertices[2].emissiveColor = color;
	vertices[3].emissiveColor = color;
	RwEngineInstance->dOpenDevice.fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
	return 1;
}

D3DXMATRIX *GetWorldTransposedMatrix(D3DXMATRIX *out, RwMatrix *world) {
	out->m[0][0] = world->right.x;
	out->m[0][1] = world->up.x;
	out->m[0][2] = world->at.x;
	out->m[0][3] = world->pos.x;

	out->m[1][0] = world->right.y;
	out->m[1][1] = world->up.y;
	out->m[1][2] = world->at.y;
	out->m[1][3] = world->pos.y;

	out->m[2][0] = world->right.z;
	out->m[2][1] = world->up.z;
	out->m[2][2] = world->at.z;
	out->m[2][3] = world->pos.z;

	out->m[3][0] = 0.0f;
	out->m[3][1] = 0.0f;
	out->m[3][2] = 0.0f;
	out->m[3][3] = 1.0f;
	return out;
}
D3DXMATRIX *GetWorldViewProj(D3DXMATRIX *mOut, RwMatrix *mWorld, D3DXMATRIX *mViewProj) {
	D3DXMATRIX mView, mProj, mWorldTransposed;
	// If we have World RwMatrix than we need to get transposed D3DXMATRIX.
	if (mWorld){
		GetWorldTransposedMatrix(&mWorldTransposed, mWorld);
	}
	// We need to get View and Projection Matricles from Device.
	g_Device->GetTransform(D3DTS_VIEW, &mView);
	g_Device->GetTransform(D3DTS_PROJECTION, &mProj);
	// We need to calculate Transposed (View * Projection) Matrix
	D3DXMatrixMultiplyTranspose(mViewProj, &mView, &mProj);
	// If there is World Matrix and Output Matrix not equal null, than we compute (World * View * Projection) Matrix.
	if (mWorld&&mOut){
		D3DXMatrixMultiply(mOut, mViewProj, &mWorldTransposed);
	}
	return mOut;
}
// Deferred Lighting Optimizations
RECT DetermineClipRect(const D3DXVECTOR3& position, const float range, D3DXMATRIX m_View, D3DXMATRIX m_Projection, float screenW, float screenH)
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
	bbox2D.top = (LONG)minY;
	bbox2D.bottom = (LONG)maxY;
	bbox2D.left = (LONG)minX;
	bbox2D.right = (LONG)maxX;

	return bbox2D;
}

HRESULT GetCurrentStates(){
	return 1;
}
HRESULT SetOldStates(){
	return 1;
}

//--------------------Full-Screen Quad Drawing Function-------------------
void DrawFullScreenQuad() {
	IDirect3DVertexDeclaration9*  VertDecl = NULL, *oldVertDecl = NULL;
	struct Vertex {
		D3DXVECTOR2 pos;
		D3DXVECTOR2 tex_coord;
	}quad[4];
	// 1) We need to create quad verticles and texture coordinates.
	quad[0].pos = D3DXVECTOR2(-1, -1); quad[0].tex_coord = D3DXVECTOR2(0, 1);
	quad[1].pos = D3DXVECTOR2(-1, 1);  quad[1].tex_coord = D3DXVECTOR2(0, 0);
	quad[2].pos = D3DXVECTOR2(1, -1);  quad[2].tex_coord = D3DXVECTOR2(1, 1);
	quad[3].pos = D3DXVECTOR2(1, 1);   quad[3].tex_coord = D3DXVECTOR2(1, 0);

	// 2) We need to create quad vertex declaration.
	const D3DVERTEXELEMENT9 Decl[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	g_Device->CreateVertexDeclaration(Decl, &VertDecl);
	// 3) Finnaly we need to set it all and draw it.
	g_Device->GetVertexDeclaration(&oldVertDecl);
	g_Device->SetVertexDeclaration(VertDecl);
	g_Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex));
	// 4) And don't forget to release it, otherwise you can crash.
	SAFE_RELEASE(VertDecl);
	g_Device->SetVertexDeclaration(oldVertDecl);
}
//------------------------------------------------------------------------

// Post-Process Drawing Pass.... TODO: idk what to do with it... maybe make passes count in value?
void DrawPostProcessPass(CEffect *m_pEffect) {
	DWORD dwOldFVF;
	const DWORD dwFVF_POST = D3DFVF_XYZRHW | D3DFVF_TEX1;
	g_Device->GetFVF(&dwOldFVF);
	g_Device->SetFVF(dwFVF_POST);
	m_pEffect->Begin();	  // This function begins pass(here we use only first pass for now).
	DrawFullScreenQuad();		  // Call Full-Screen Quad Drawing Function to draw Full-Screen Quad!
	m_pEffect->End();		  // This function ends current pass. Don't forget it!
	g_Device->SetFVF(dwOldFVF);
}
void DrawPostProcessPass(ID3DXEffect *m_pEffect) {
	DWORD dwOldFVF;
	UINT pss;
	const DWORD dwFVF_POST = D3DFVF_XYZRHW | D3DFVF_TEX1;
	g_Device->GetFVF(&dwOldFVF);
	g_Device->SetFVF(dwFVF_POST);
	m_pEffect->Begin(&pss, 0);	  // This function begins pass(here we use only first pass for now).
	m_pEffect->BeginPass(0);
	DrawFullScreenQuad();		  // Call Full-Screen Quad Drawing Function to draw Full-Screen Quad!
	m_pEffect->EndPass();
	m_pEffect->End();		  // This function ends current pass. Don't forget it!
	g_Device->SetFVF(dwOldFVF);
}