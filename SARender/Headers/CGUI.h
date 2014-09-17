/********************************SA::Render source file*************************************/
/* File name   : CGUI.h                                                                    */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CGUI controls SA::Render user interface.                                  */
/* File created: 12.05.2014                                                                */
/* File last ed: 12.05.2014                                                                */
/*******************************************************************************************/
#pragma once
#include "AntTweakBar.h"
#include "CEffectMgr.h"
#include "CGame.h"

class CGUI
{
public:
	CGUI();
	void Initialize();
	void Shutdown();
	void AddVar(const char* name, TwType type, const void* Var);
public:
	static TwBar *m_pGUI;
};

extern CGUI gGUI;