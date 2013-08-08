/*
    SA::Render source file
    CAutomobilePanel structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct CAutomobilePanel
{
    short m_sFrameId;
    short m_sAxis;
    float m_fAngleLimit;
    RwV3D m_vRotation;
    RwV3D m_vPos;
};
#pragma pack(pop)