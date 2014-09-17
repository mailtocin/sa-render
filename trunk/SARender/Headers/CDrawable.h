/********************************SA::Render source file*************************************/
/* File name   : CDrawable.h                                                               */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CDrawable controls drawing algorithm.						               */
/* File created: 12.05.2014                                                                */
/* File last ed: 13.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "game_sa\RenderWare.h"
#include "patch\CPatch.h"
#include "CEffect.h"
#include "CGlobalsMgr.h"

class CDrawable
{
	static void RenderCallBack(RwResEntry *RepEntry, RpAtomic *Atomic, unsigned char Type, char Flags);
public:
	static void Patch();
	static void Initialize();
	static void Render(RxD3D9ResEntryHeader *Header, RxD3D9InstanceData *Mesh, CEffect* Effect);
	static void Shutdown();
	static CEffect *m_pEffect;
};