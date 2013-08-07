//
#include "RenderWare.h"
#include "CPatch.h"
//

//Func
void __fastcall VehicleRenderPassengers(void *vehicle)
{
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
	((void ( __fastcall *)(void *))0x6D3D60)(vehicle);
}
//

//Setup
void CullRenderSetup()
{
	CPatch::Nop(0x5532B7, 2);
	CPatch::RedirectCall(0x5532A2, VehicleRenderPassengers);
}
//