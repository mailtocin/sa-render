/*
    SA::Render source file
    CAudioManager structure
*/
#pragma once
#include "CAudioWav.h"
#include "CAudioLink.h"

#pragma pack(push, 1)
struct CAudioManager
{
    CAudioWav field_0[4];
    int field_80;
    int field_84;
    float field_88;
    float field_8C;
    CAudioLink wavLinks[4];
    int field_110;
    int field_114;
    int __unk4;
    int field_11C;
    char gap_120[4];
    int field_124;
    char field_128[104];
    int field_190;
    int field_194;
    int field_198;
    int field_19C;
    int __unk5;
    int field_1A4;
    char gap_1A8[4];
    int field_1AC;
    char __unk6[104];
    char field_218;
    char field_219[3];
};
#pragma pack(pop)