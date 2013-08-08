/*
    SA::Render source file
    CAudioLink structure
*/
#pragma once
#include "RenderWare.h"
#include "CEntity.h"

#pragma pack(push, 1)
struct CAudioLink
{
    RwV3D m_vPosition;
    CEntity *m_pEntity;
    int field_10;
    int field_14;
    int m_nBankId;
    int m_nBankSlotId;
};
#pragma pack(pop)