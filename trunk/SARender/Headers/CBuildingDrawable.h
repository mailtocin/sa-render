/********************************SA::Render source file*************************************/
/* File name   : CBuildingDrawable.h                                                       */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CBuildingDrawable controls building drawing algorithm.                    */
/* File created: 12.05.2014                                                                */
/* File last ed: 15.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CDrawable.h"
class CBuildingDrawable : 
	public CDrawable
{
	static void RenderCallBack(RwResEntry *RepEntry, RpAtomic *Atomic, unsigned char Type, char Flags);
public:
	static void Patch();
	static void Initialize();
	static CEffect *m_pEffect;
};

