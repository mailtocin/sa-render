/*
    SA::Render source file
    CWeapon structure
*/
#pragma once
#include "eWeaponType.h"

#pragma pack(push, 1)
struct CWeapon
{
    eWeaponType type;
    int state;
    int ammoInClip;
    int ammoRemaining;
    int m_dwTimeForNextShot;
    char field_14;
    char field_15;
    char field_16;
    char field_17;
    int m_pParticle;
};
#pragma pack(pop)