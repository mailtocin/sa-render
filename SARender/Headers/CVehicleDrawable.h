/********************************SA::Render source file*************************************/
/* File name   : CVehicleDrawable.h                                                        */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CVehicleDrawable controls vehicle drawing algorithm.                      */
/* File created: 11.05.2014                                                                */
/* File last ed: 13.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CDrawable.h"

class CVehicleDrawable :public CDrawable
{
	static void RenderCallBack(RwResEntry *RepEntry, RpAtomic *Atomic, unsigned char Type, char Flags);
public:
	static void Patch();
	static void Initialize();
	static CEffect *m_pEffect;
	static RwTexture *m_pBurnoutTex, *m_pRaindropTex;
};

