/*
    SA::Render source file
    CEntityBase structure
*/
#pragma once
#include "CPlaceableBase.h"
#include "RenderWare.h"
#include "eEntityFlags.h"
#include "CEntity.h"

#pragma pack(push, 1)
struct CEntityBase
{
    CPlaceableBase placeable;
    RpClump *m_pRWObject;
    eEntityFlags m_dwFlags;
    short m_wRandomSeed;
    short m_wModelIndex;
    int m_pReferences;
    void *m_pLastRenderedLink;
    short m_nScanCode;
    char m_iplIndex;
    char m_nbInterior;
    struct CEntity *m_pLod;
    char m_bNumLodChildren;
    char m_bNumLodChildrenRendered;
    char m_bTypeState;
    char _pad0;
};
#pragma pack(pop)