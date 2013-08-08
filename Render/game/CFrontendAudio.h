/*
    SA::Render source file
    CFrontendAudio structure
*/
#pragma once

#pragma pack(push, 1)
struct CFrontendAudio
{
    unsigned char f0[124];
    unsigned char byte7C;
    unsigned char byte7D;
    unsigned char f7E[2];
    unsigned short m_nNumBulletSounds;
    unsigned short bankId;
    unsigned int m_dwLastFrameGeneral;
    unsigned int m_dwLastFrameMissionComplete;
    unsigned int m_dwLastFrameBulletPass;
    unsigned int m_dwLastTimeCarRespray;
    unsigned int dword94;
    unsigned int dword98;
    unsigned char f9C[136];
    unsigned short word124;
    unsigned char f126[166];
    unsigned short word1CC;
};
#pragma pack(pop)