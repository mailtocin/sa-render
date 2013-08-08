/*
    SA::Render source file
    cDMAudio structure
*/
#pragma once
#include "CFrontendAudio.h"
#include "CAudioManager.h"
#include "CGlassAudio.h"

#pragma pack(push, 1)
struct cDMAudio
{
    short field_0;
    char m_nCurrentRadiostationId;
    char field_3;
    int field_4;
    char gap_8[152];
    char field_A0;
    char gap_A1[15];
    char field_B0;
    char gap_B1[3];
    CFrontendAudio m_FrontendAudio;
    char gap_282[30];
    CAudioManager audioManager;
    CGlassAudio m_TempAudio;
};
#pragma pack(pop)