/********************************SA::Render source file*************************************/
/* File name   : CObjectDrawable.h                                                         */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CObjectDrawable controls object drawing algorithm.                        */
/* File created: 15.05.2014                                                                */
/* File last ed: 15.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CDrawable.h"
class CObjectDrawable :
	public CDrawable
{
	static void RenderCallBack(RwResEntry *RepEntry, RpAtomic *Atomic, unsigned char Type, char Flags);
public:
	static void Patch();
	static void Initialize();
	static CEffect *m_pEffect;
};

