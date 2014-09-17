/********************************SA::Render source file*************************************/
/* File name   : CRenderTarget.h                                                           */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CRenderTarget is render target class.								       */
/* File created: 13.05.2014                                                                */
/* File last ed: 13.05.2014                                                                */
/*******************************************************************************************/
#pragma once

#include <list>
#include <d3dx9.h>
class CRenderTarget;

class CRTMgr
{
	std::list<CRenderTarget *> m_RTs;
	IDirect3DSurface9 * m_pOldSurf;
	IDirect3DSurface9 * m_pOldZSurf;
public:
	void Initialise();
	void AddRT(CRenderTarget *pRT);
	void RemoveRT(CRenderTarget *pRT);
	void Shutdown();
	void Lost();
	void Reset();
	void GetRef();
	void ResetRef();
	void GetZRef();
	void ResetZRef();
};

extern CRTMgr gRTMgr;