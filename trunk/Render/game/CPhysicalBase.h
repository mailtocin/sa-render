/*
    SA::Render source file
    CPhysicalBase structure
*/
#pragma once
#include "CEntityBase.h"
#include "RenderWare.h"
//#include "CShadowData.h"

#pragma pack(push, 1)
struct CPhysicalBase
{
    CEntityBase entity;
    int field_38;
    int field_3C;
    int immunities;
    RwV3D m_vVelocity;
    RwV3D m_vAngularVelocity;
    RwV3D m_vAcceleration;
    RwV3D m_vAngularAcceleration;
    RwV3D m_vForce;
    RwV3D m_vTorque;
    float m_fMass;
    int m_fTurnMass;
    int m_fVelocityFrequency;
    float fAirResistance;
    float m_fElasticity;
    float m_fBuoyancyConstant;
    RwV3D CentreOfMass;
    int field_B0;
    int field_B4;
    char field_B8;
    char numEntitiesCollided;
    char contactSurface;
    char field_BB;
    int collidedEntities[6];
    int field_D4;
    int DamageIntensity;
    int DamageEntity;
    RwV3D LastCollisionDirection;
    RwV3D LastCollision;
    short DamagedPart;
    short field_FA;
    int AttachedTo;
    RwV3D AttachOffset;
    RwV3D AttachRotation;
    int field_118;
    int field_11C;
    int field_120;
    int field_124;
    int EntityIgnoredCollision;
    float collisionSurfaceBrightness;
    int field_130;
    void *shadowData;
};
#pragma pack(pop)