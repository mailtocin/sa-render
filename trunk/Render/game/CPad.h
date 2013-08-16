/*
    SA::Render source file
    CPad structure
*/
#pragma once
#include "PadData.h"

#pragma pack(push, 1)
struct CPad
{
    CControllerState padState;
    CControllerState prevPadState;
    short field_60[10];
    int field_74;
    CControllerState padStateKbd1;
    CControllerState padStateKbd2;
    CControllerState field_D8;
    char field_108;
    char field_109;
    short field_10A;
    short field_10C;
    short field_10E;
    char field_110;
    int field_111;
    char field_115;
    char field_116;
    char menuActive;
    char field_118;
    char field_119;
    char field_11A;
    char field_11B;
    char field_11C;
    char field_11D;
    char field_11E;
    char field_11F;
    int padQueryTime;
    int field_124;
    int field_128;
    int field_12C;
    char field_130;
    char field_131;
    short field_132;
};
#pragma pack(pop)