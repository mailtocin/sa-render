/*
    SA::Render source file
    CBone structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct _CBone
{
    char m_bFlags;
    char _padding0[3];
    RwV3D m_vOffset;
    RtQuat *m_pQuat;
    int m_dwNodeId;
};
#pragma pack(pop)