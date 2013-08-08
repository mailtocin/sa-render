/*
    SA::Render source file
    CTransmissionData structure
*/
#pragma once
#include "TransmissionGear.h"

#pragma pack(push, 1)
struct CTransmissionData
{
    TransmissionGear gears[6];
    char nDriveType;
    char nEngineType;
    char nNumberOfGears;
    char field_4B;
    int field_4C;
    int fEngineAcceleration;
    int fEngineInertia;
    int fMaxVelocity;
    int field_5C;
    int field_60;
    int field_64;
};
#pragma pack(pop)