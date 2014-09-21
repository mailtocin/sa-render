/********************************SA::Render source file*************************************/
/* File name   : CDrawable.cpp                                                             */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CDrawable controls drawing algorithm.						               */
/* File created: 12.05.2014                                                                */
/* File last ed: 19.09.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CDrawable.h"

CEffect *CDrawable::m_pEffect;

void CDrawable::Render(RxD3D9ResEntryHeader *Header, RxD3D9InstanceData *Mesh, CEffect* Effect){
	rwD3D9RenderStateFlushCache();
	Effect->Begin();
	/*rwD3D9SetRenderState(D3DRS_FOGENABLE, 0);
	*/
	//nStateChanges = nTexStageStateChanges = 0;
	if (Header->indexBuffer)
		rwD3D9DrawIndexedPrimitive(Header->primType, Mesh->baseIndex, 0, Mesh->numVertices, Mesh->startIndex, Mesh->numPrimitives);
	else
		rwD3D9DrawPrimitive(Header->primType, Mesh->baseIndex, Mesh->numPrimitives);
	Effect->End();
}
void CDrawable::Shutdown()
{
	delete m_pEffect;
}
