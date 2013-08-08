/*
    SA::Render source file
    CAutomobile structure
*/
#pragma once
#include "CVehicle_vtbl.h"
#include "CAutomobileBase.h"

#pragma pack(push, 1)
struct CAutomobile
{
    CVehicle_vtbl *vmt;
    CAutomobileBase automobile;
};
#pragma pack(pop)