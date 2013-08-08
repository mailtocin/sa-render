/*
    SA::Render source file
    CHandlingVehicle structure
*/
#pragma once
#include "RenderWare.h"
#include "CTransmissionData.h"

#pragma pack(push, 1)
struct CHandlingVehicle
{
    int index;
    float fMass;
    int field_8;
    float fTurnMass;
    float fDragMult;
    RwV3D centreOfMass;
    char nPercentSubmerged;
    char field_21;
    char field_22;
    char field_23;
    float fBuoyancyConstant;
    int fTractionMultiplier;
    CTransmissionData transmissionData;
    int fBrakeDeceleration;
    int fBrakeBias;
    char bABS;
    char field_9D;
    char field_9E;
    char field_9F;
    int fSteeringLock;
    int fTractionLoss;
    int fTractionBias;
    int fSuspensionForceLevel;
    int fSuspensionDampingLevel;
    int fSuspensionHighSpdComDamp;
    float fSuspensionUpperLimit;
    float fSuspensionLowerLimit;
    int fSuspensionBiasBetweenFrontAndRear;
    int fSuspensionAntiDiveMultiplier;
    int fCollisionDamageMultiplier;
    int modelFlags;
    int handlingFlags;
    int fSeatOffsetDistance;
    int nMonetaryValue;
    char frontLight;
    char rearLight;
    char animGroup;
    char field_DF;
};
#pragma pack(pop)