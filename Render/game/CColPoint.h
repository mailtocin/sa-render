/*
    SA::Render source file
    CColPoint structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct CColPoint
{
    RwV3D point;
    int field_C;
    RwV3D normal;
    int field_1C;
    char surfaceTypeA;
    char pieceTypeA;
    char lightingA;
    char surfaceTypeB;
    char pieceTypeB;
    char lightingB;
    char field_26[2];
    float depth;
};
#pragma pack(pop)