#include "CGame.h"

bool GetSunPosn(CVector *dst)
{
	float sunDistance;
	CVector *sunPos, *camPos;
	bool res;
	sunDistance = *(float*)0xC3EF9C * 0.95f;
	sunPos = (CVector *)(*(DWORD *)0xB79FD0 * 12 + 0xB7CA50);
	if(sunPos->z > -0.1f)
	res = TRUE;
	else
	res = FALSE;
	if(*(DWORD*)0xB6F03C)
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

bool GetSunPosn(CVector *dst, float farClip)
{
	CVector *sunPos, *camPos;
	bool res;
	sunPos = (CVector *)(*(DWORD *)0xB79FD0 * 12 + 0xB7CA50);
	if(sunPos->z > -0.1f)
	res = TRUE;
	else
	res = FALSE;
	if(*(DWORD*)0xB6F03C)
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
	return res;
}

CVector *GetCamPos()
{
	return &TheCamera->m_vecGameCamPos;
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

D3DXMATRIX *getWorldViewProj(D3DXMATRIX *out, RwMatrix *world, D3DXMATRIX *viewProj) {
 D3DXMATRIX view,proj,worldtransp;
	worldtransp.m[0][0] = world->right.x;
	worldtransp.m[0][1] = world->up.x;
	worldtransp.m[0][2] = world->at.x;
	worldtransp.m[0][3] = world->pos.x;

	worldtransp.m[1][0] = world->right.y;
	worldtransp.m[1][1] = world->up.y;
	worldtransp.m[1][2] = world->at.y;
	worldtransp.m[1][3] = world->pos.y;

	worldtransp.m[2][0] = world->right.z;
	worldtransp.m[2][1] = world->up.z;
	worldtransp.m[2][2] = world->at.z;
	worldtransp.m[2][3] = world->pos.z;

	worldtransp.m[3][0] = 0.0f;
	worldtransp.m[3][1] = 0.0f;
	worldtransp.m[3][2] = 0.0f;
	worldtransp.m[3][3] = 1.0f;

   g_Device->GetTransform(D3DTS_VIEW,&view);
   g_Device->GetTransform(D3DTS_PROJECTION,&proj);
   D3DXMatrixMultiplyTranspose(viewProj,&view,&proj);
   D3DXMatrixMultiply(out,viewProj,&worldtransp);
 return out;
}