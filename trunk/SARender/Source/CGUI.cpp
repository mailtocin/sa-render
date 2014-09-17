/********************************SA::Render source file*************************************/
/* File name   : CGUI.cpp                                                                  */
/* File creator: PetkaGtA                                                                  */
/* File editors: PetkaGtA                                                                  */
/* File descrip: CGUI controls SA::Render user interface.                                  */
/* File created: 12.05.2014                                                                */
/* File last ed: 12.05.2014                                                                */
/*******************************************************************************************/
#include "..\Headers\CGUI.h"

TwBar *CGUI::m_pGUI;

void TW_CALL ReloadShaders(void *clientData)
{
	gEffectMgr.Reload();
}

CGUI::CGUI()
{
	m_pGUI = NULL;
}

void CGUI::Initialize()
{
	TwInit(TW_DIRECT3D9, g_Device);

	m_pGUI = TwNewBar("SA::Render");

	TwAddButton(m_pGUI, "ReloadShaders", ReloadShaders, NULL, "group=Global");
}
void CGUI::AddVar(const char* name, TwType type, const void* Var){
	TwAddVarRO(m_pGUI, name, type, Var,"");
}
void CGUI::Shutdown()
{
	TwTerminate();
}

CGUI gGUI;