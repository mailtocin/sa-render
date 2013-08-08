/*
    SA::Render source file
    CPlayerData structure
*/
#pragma once
#include "CWanted.h"
#include "CClothes.h"
#include "CVector.h"

#pragma pack(push, 1)
struct CPlayerData
{
    CWanted *m_pWanted;
    CClothes *m_pClothes;
    int field_8;
    int field_C;
    int field_10;
    int m_nCrimeSensitivity;
    float m_fTimeCanRun;
    int field_1C;
    char field_20;
    char field_21;
    short __pad1;
    int _Idle2StartScreenSaver;
    int field_28;
    int field_2C;
    int field_30;
    int flags;
    int _recruitesIndex;
    int nAdrenalineTime;
    char _isDrunk;
    char field_41;
    char field_42;
    char field_43;
    float fBreathMeter;
    int field_48;
    int field_4C;
    int field_50;
    int field_54;
    int field_58;
    int field_5C;
    float m_fMinigunRotation;
    int field_64;
    int field_68;
    int field_6C;
    int field_70;
    CVector field_74;
    int field_80;
    char field_84;
    char bUsingCellphone;
    char field_86;
    char field_87;
    short field_88;
    short field_8A;
    char field_8C;
    char field_8D;
    char field_8E;
    char field_8F;
    int field_90;
    int field_94;
    int field_98;
    int lastModelShot;
    float field_A0;
    int field_A4;
    int field_A8;
};
#pragma pack(pop)