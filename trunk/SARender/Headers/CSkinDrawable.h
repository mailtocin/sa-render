/********************************SA::Render source file*************************************/
/* File name   : CSkinDrawable.h							                               */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CSkinDrawable controls peds and player drawing algorithm.                 */
/* File created: 15.05.2014                                                                */
/* File last ed: 15.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "CDrawable.h"
class CSkinDrawable :
	public CDrawable
{
public:
	static void Patch();
	static CEffect *m_pEffect;
};

