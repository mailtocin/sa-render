/*
    SA::Render source file
    CStoredCollPoly structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct CStoredCollPoly
{
    RwV3D meshVertexA;
    RwV3D meshVertexB;
    RwV3D meshVertexC;
    char isActual;
    char _padding[3];
    int lighting;
};
#pragma pack(pop)