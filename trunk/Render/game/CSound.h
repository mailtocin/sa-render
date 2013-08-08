/*
    SA::Render source file
    CSound structure
*/
#pragma once
//#include "CVehicleAudio.h"
#include "CEntity.h"
#include "eAudioEvents.h"
#include "CVector.h"

#pragma pack(push, 1)
struct CSound
{
    unsigned short m_nBankSlotId;
    short m_nSoundIdInSlot;
    void *m_pBaseAudio;
    CEntity *m_pEntity;
    eAudioEvents m_nEventId;
    float m_dwTimeToPlay;
    float m_fVolume;
    float field_18;
    float m_fSpeed;
    float field_20;
    CVector m_vLastPosn;
    CVector m_vCurrPosn;
    unsigned int m_dwLastFrameUpdate;
    int m_dwLastTime;
    int m_dwCurrTime;
    float m_fLastDistance;
    float m_fCurrDistance;
    float field_50;
    unsigned char field_54;
    unsigned char field_55[1];
    unsigned short m_nFlags;
    unsigned short m_wIsUsed;
    short field_5A;
    unsigned short field_5C;
    unsigned short field_5E;
    float field_60;
    float field_64;
    unsigned short field_68;
    unsigned char field_6A[2];
    float field_6C;
    unsigned short field_70;
    short field_72;
};
#pragma pack(pop)