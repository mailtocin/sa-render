#include "CImmediateRender.h"
#include "CPatch.h"
#include <d3d9.h>

eImmediateRendering CImmediateRender::m_nCurrentRendering;

void CImmediateRender::Patch()
{
	CPatch::Nop(0x80E560, 1);
	CPatch::RedirectCall(0x80E561, DrawPrimitive_1);
	CPatch::Nop(0x80E8D3, 1);
	CPatch::RedirectCall(0x80E8D4, DrawPrimitive_2);
	CPatch::Nop(0x80E931, 1);
	CPatch::RedirectCall(0x80E932, DrawPrimitiveUp);
	CPatch::Nop(0x80E707, 1);
	CPatch::RedirectCall(0x80E708, DrawIndexedPrimitive);
	CPatch::Nop(0x80E7B2, 1);
	CPatch::RedirectCall(0x80E7B3, DrawIndexedPrimitiveUp);
}

HRESULT __fastcall CImmediateRender::DrawPrimitive_1(int _ecx, int _edx, IDirect3DDevice9 *device, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, 
	UINT PrimitiveCount)
{
	HRESULT result;
	// --
	result = device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
	// --
	return result;
}

HRESULT __fastcall CImmediateRender::DrawPrimitive_2(int _ecx, int _edx, IDirect3DDevice9 * device, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, 
	UINT PrimitiveCount)
{
	HRESULT result;
	// --
	result = device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
	// --
	return result;
}

HRESULT __fastcall CImmediateRender::DrawPrimitiveUp(int _ecx, int _edx, IDirect3DDevice9 * device, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,
	const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	HRESULT result;
	// --
	result = device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	// --
	return result;
}

HRESULT __fastcall CImmediateRender::DrawIndexedPrimitive(int _ecx, int _edx, IDirect3DDevice9 *device, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, 
	UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	HRESULT result;
	// --
	result = device->DrawIndexedPrimitive(Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	// --
	return result;
}

HRESULT __fastcall CImmediateRender::DrawIndexedPrimitiveUp(int _ecx, int _edx, IDirect3DDevice9 *device, D3DPRIMITIVETYPE PrimitiveType,
	UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexStreamZeroData,
	UINT VertexStreamZeroStride)
{
	HRESULT result;
	// --
	result = device->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData,
		VertexStreamZeroStride);
	// --
	return result;
}