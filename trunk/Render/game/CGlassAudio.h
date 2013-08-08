/*
    SA::Render source file
    CGlassAudio structure
*/
#pragma once
#include "CSound.h"

#pragma pack(push, 1)
struct CGlassAudio
{
    unsigned char f0[8];
    CSound sound;
};
#pragma pack(pop)