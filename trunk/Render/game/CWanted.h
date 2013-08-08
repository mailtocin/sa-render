/*
    SA::Render source file
    CWanted structure
*/
#pragma once
#include "CWantedCop.h"

#pragma pack(push, 1)
struct CWanted
{
    int Chaos;
    int field_4;
    int field_8;
    int timeWhenGotStars;
    int field_10;
    float field_14;
    char ncops;
    char maxcops;
    char nhelis;
    char field_1B;
    short field_1C;
    char field_1E;
    char field_1F;
    int field_20;
    int field_24;
    int field_28;
    int currentWantedLevel;
    int maxWantedLevel;
    CWantedCop cops[16];
    int _cops_vmt[10];
    int field_21C;
    int field_220;
    int field_224;
    int field_228;
    int field_22C;
    int field_230;
    int field_234;
    int field_238;
    int field_23C;
    int field_240;
    int field_244;
    int field_248;
    int field_24C;
    int field_250;
    int field_254;
    int field_258;
    int field_25C;
    int field_260;
    int field_264;
    int field_268;
    int field_26C;
    int field_270;
    int field_274;
    int field_278;
    int field_27C;
    int field_280;
    int field_284;
    int field_288;
    int field_28C;
    int field_290;
    int field_294;
    int field_298;
};
#pragma pack(pop)