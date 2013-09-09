/********************************SA::Render source file*************************************/
/* File creator: DK22Pac                                                                   */
/* File editors: DK22Pac                                                                   */
/* File descrip: Dll attaching.                                                            */
/* File created: 17.04.2013                                                                */
/* File last ed: 06.05.2013                                                                */
/*******************************************************************************************/
#include <Windows.h>
#include "CRender.h"
#include "CDebug.h"
#include "Initialise.h"

void RenderAttach();

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    if(reason == DLL_PROCESS_ATTACH){
		InitialiseRender(CRender::Patch);
	}
    return TRUE;
}