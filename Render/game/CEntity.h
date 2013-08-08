/*
    SA::Render source file
    CEntity structure
*/
#pragma once
#include "CEntity_vtbl.h"
#include "CEntityBase.h"

#pragma pack(push, 1)
struct CEntity
{
    CEntity_vtbl *_vmt;
    CEntityBase entity;
};
#pragma pack(pop)