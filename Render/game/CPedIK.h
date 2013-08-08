/*
    SA::Render source file
    CPedIK structure
*/
#pragma once
#include "LimbOrienation.h"

#pragma pack(push, 1)
struct CPedIK
{
    int ThisPed;
    LimbOrienation m_torsoOrien;
    float m_fSlopePitch;
    float m_fSlopePitchLimitMult;
    float m_fSlopeRoll;
    float m_fBodyRoll;
    int m_flags;
};
#pragma pack(pop)