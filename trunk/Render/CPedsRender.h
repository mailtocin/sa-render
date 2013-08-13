#pragma once
#include "RenderWare.h"
#include "D3D9Headers\d3dx9.h"
#include "CGame.h"

class CPedsRender
{
	static HRESULT rxD3D9VertexShaderDefaultMeshRenderCallBack(RxD3D9ResEntryHeader *resEntry, RxD3D9InstanceData *instData);
public:
	static void Patch();
	static bool Setup();
	static void Reset();
	static void Lost();
	static ID3DXEffect *m_pEffect;
	static D3DXMATRIX m_LightViewProj;
};