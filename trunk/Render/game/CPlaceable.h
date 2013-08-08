/*
    SA::Render source file
    CPlaceable structure
*/
#pragma once
#include "CPlaceableBase.h"

#pragma pack(push, 1)
struct CPlaceable
{
    int _vmt;
    CPlaceableBase placeable;
};
#pragma pack(pop)