#include "CGrassRender.h"
#include <d3d9.h>

HRESULT DrawGrass(int _ecx, int _edx, IDirect3DDevice9 *device, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, 
							UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount){
	g_Device->DrawIndexedPrimitive(Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
	return TRUE;
}
