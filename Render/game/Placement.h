/*
    SA::Render source file
    Placement structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct Placement
{
    RwV3D pos;
    float angle;
};
#pragma pack(pop)