/*
    SA::Render source file
    CPhysical structure
*/
#pragma once
#include "CVehicle_vtbl.h"
#include "CPhysicalBase.h"

#pragma pack(push, 1)
struct CPhysical
{
    CVehicle_vtbl *_vmt;
    CPhysicalBase physical;
};
#pragma pack(pop)