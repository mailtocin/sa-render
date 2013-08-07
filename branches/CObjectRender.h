#pragma once
#include "RenderWare.h"
#include <d3dx9.h>

class CObjectRender
{
	static void NvcRenderCB(RwResEntry *repEntry, RpAtomic *atomic, unsigned char type, char flags);
	static HRESULT __fastcall DefaultRender_DrawIndexedPrimitiveA(int ecx0,
																  int edx0,
																  IDirect3DDevice9 *device,
																  D3DPRIMITIVETYPE Type,
																  INT BaseVertexIndex,
																  UINT MinIndex,
																  UINT NumVertices,
																  UINT StartIndex,
																  UINT PrimitiveCount);
	static HRESULT __fastcall DefaultRender_DrawIndexedPrimitiveB(int ecx0,
																  int edx0,
																  IDirect3DDevice9 *device,
																  D3DPRIMITIVETYPE Type,
																  INT BaseVertexIndex,
																  UINT MinIndex,
																  UINT NumVertices,
																  UINT StartIndex,
																  UINT PrimitiveCount);
	static HRESULT __fastcall DefaultRender_DrawPrimitiveA(int ecx0,
                                 int edx0,
         IDirect3DDevice9 *device,
         D3DPRIMITIVETYPE PrimitiveType,
         UINT StartVertex,
         UINT PrimitiveCount);
	static HRESULT __fastcall DefaultRender_DrawPrimitiveB(int ecx0,
                                 int edx0,
         IDirect3DDevice9 *device,
         D3DPRIMITIVETYPE PrimitiveType,
         UINT StartVertex,
         UINT PrimitiveCount);
	static void DefaultRender_Callback(RwResEntry *resEntry, RpAtomic *atomic, int bClipSphere, int flags);
	static void Update();
public:
	static void Patch();
	static bool Setup();
	static void Reset();
	static void Lost();
	static ID3DXEffect *m_pEffect;
	static D3DXMATRIX m_LightViewProj;
	static D3DXMATRIX m_LightViewProj2;
	static bool bWeaponRender;
	static RpAtomic *g_DefaultRender_Atomic;
	static RwResEntry *g_DefaultRender_ResEntry;
	static int g_DefaultRender_ClipSphere;
	static int g_DefaultRender_Flags;
};