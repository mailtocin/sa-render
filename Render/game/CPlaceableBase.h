/*
    SA::Render source file
    CPlaceableBase structure
*/
#pragma once
#include "Placement.h"
#include "CMatrix.h"

#pragma pack(push, 1)
struct CPlaceableBase
{
    Placement placement;
    CMatrix *m_pCoords;
};
#pragma pack(pop)