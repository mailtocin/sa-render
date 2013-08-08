/*
    SA::Render source file
    CVehicleSound structure
*/
#pragma once
#include "CSound.h"

#pragma pack(push, 1)
struct CVehicleSound
{
    int m_nIndex;
    struct CSound *m_pSound;
};
#pragma pack(pop)