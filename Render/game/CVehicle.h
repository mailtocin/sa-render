/*
    SA::Render source file
    CVehicle structure
*/
#pragma once
#include "CVehicle_vtbl.h"
#include "CVehicleBase.h"

#pragma pack(push, 1)
struct CVehicle
{
    CVehicle_vtbl *_vmt;
    CVehicleBase vehicle;
};
#pragma pack(pop)