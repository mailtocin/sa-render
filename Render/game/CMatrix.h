/*
    SA::Render source file
    CMatrix structure
*/
#pragma once
#include "RenderWare.h"
#include "CMatrix.h"

#pragma pack(push, 1)
struct CMatrix
{
    RwMatrix matrix;
    CMatrix *matrixPtr;
    int haveRwMatrix;
};
#pragma pack(pop)