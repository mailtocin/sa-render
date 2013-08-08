/*
    SA::Render source file
    CClothes structure
*/
#pragma once

#pragma pack(push, 1)
struct CClothes
{
    int m_aModelKeys[10];
    int m_aTextureKeys[18];
    float m_fFatStat;
    float m_fMuscleStat;
};
#pragma pack(pop)