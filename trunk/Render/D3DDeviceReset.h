#pragma once
#include <Windows.h>
__declspec(dllimport) bool _D3DDeviceResetAdd(void (*func)(), bool bBeforeReset);

void D3DDeviceResetAdd(void (*func)(), bool bBeforeReset)
{
	LoadLibrary("D3DDeviceReset.dll");
	_D3DDeviceResetAdd(func, bBeforeReset);
}