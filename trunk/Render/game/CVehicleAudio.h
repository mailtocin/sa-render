/*
    SA::Render source file
    CVehicleAudio structure
*/
#pragma once
//#include "CVehicle.h"
#include "CSound.h"
#include "VehicleAudioProperties.h"
#include "CVehicleSound.h"

#pragma pack(push, 1)
struct CVehicleAudio
{
    unsigned int vmt;
    void *m_pVehicle;
    CSound m_sSound;
    short field_7C;
    char gap_7E[2];
    VehicleAudioProperties m_sProperties;
    unsigned char field_A4;
    char field_A5;
    char field_A6;
    char field_A7;
    char field_A8;
    char field_A9;
    char field_AA;
    char gap_AB[1];
    int field_AC;
    char UnkModelFlag;
    char gap_B1[1];
    short field_B2;
    short field_B4;
    char gap_B6[2];
    int field_B8;
    char field_BC;
    char field_BD;
    char gap_BE[6];
    float field_C4;
    char m_bModelWithSiren;
    char gap_C9[3];
    int m_dwBoatHitWaveLastPlayedTime;
    int field_D0;
    int field_D4;
    float m_fGeneralVehicleSoundVolume;
    short m_wEngineDecelerateSoundBankId;
    short m_wEngineAccelerateSoundBankId;
    short m_nEngineBankSlotId;
    short field_E2;
    CVehicleSound m_aVehicleSounds[12];
    int field_144;
    short field_148;
    short field_14A;
    short field_14C;
    short field_14E;
    char gap_150[4];
    short field_154;
    short field_156;
    CSound *field_158;
    short field_15C;
    char gap_15E[2];
    CSound *field_160;
    short field_164;
    char gap_166[2];
    CSound *field_168;
    short field_16C;
    char gap_16E[2];
    CSound *field_170;
    char gap_174[4];
    CSound *field_178;
    CSound *field_17C;
    CSound *field_180;
    unsigned int obj85F438;
    unsigned char field_188[12];
    unsigned int field_194;
    unsigned char field_198[116];
    unsigned short field_20C;
    unsigned char field_20E[22];
    unsigned int field_224;
    unsigned int field_228;
    float field_22C;
    float field_230;
    float field_234;
    float field_238;
    float field_23C;
    int field_240;
    char field_244;
    char field_245[3];
    float field_248;
};
#pragma pack(pop)