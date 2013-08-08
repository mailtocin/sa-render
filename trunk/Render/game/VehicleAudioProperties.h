/*
    SA::Render source file
    VehicleAudioProperties structure
*/
#pragma once

#pragma pack(push, 1)
struct VehicleAudioProperties
{
    char m_bVehicleType;
    char field_1;
    short m_wEngineOnSoundBankId;
    short m_wEngineOffSoundBankId;
    char m_bStereo;
    char field_7;
    int field_8;
    int field_C;
    char m_bHornTon;
    char field_11[3];
    int m_fHornHigh;
    char m_bDoorSound;
    char field_19;
    char m_bRadioNum;
    char m_bRadioType;
    char field_1C;
    char field_1D[3];
    int field_20;
};
#pragma pack(pop)