/********************************SA::Render source file*************************************/
/* File name   : CDeferredMgr.h                                                            */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CDeferredMgr deferred rendering class.									   */
/* File created: 15.06.2014                                                                */
/* File last ed: 15.06.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CGame.h"
#include "CRTMgr.h"
class CDeferredMgr
{
public:
	static CRenderTarget *m_pGeomtryBuffer[3];
	static CRenderTarget *m_pLightingBuffer;
	static CEffect *m_pEffect;
	static ID3DXMesh* PointLightMesh;
	static void Initialize();
	static void DrawLight(ID3DXMesh *pMesh, D3DXVECTOR4* vPos);
	static void RenderToGeometryBuffer(void(*render)());
	static void RenderToScreenOutput();
	static void ReInitialize();
	static void ShutDown();
};

