/*
    SA::Render source file
    CWantedCop structure
*/
#pragma once
#include "CPed.h"
#include "RenderWare.h"

#pragma pack(push, 1)
struct CWantedCop
{
    int m_dwCrimeType;
    CPed *m_pPed;
    int m_dwStartTime;
    RwV3D m_vPosition;
    char field_18;
    char m_bIsPedCriminal;
    char _pad[2];
};
#pragma pack(pop)