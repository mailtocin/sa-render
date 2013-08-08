/*
    SA::Render source file
    CDamageManager structure
*/
#pragma once
#include "CComponentStatus.h"

#pragma pack(push, 1)
struct CDamageManager
{
    float WheelDamageEffect;
    char engineStatus;
    char wheelsStatus[4];
    CComponentStatus componentStatus;
    char _pad1;
    int lightStatus;
    int panelStatus;
};
#pragma pack(pop)