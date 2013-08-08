/*
    SA::Render source file
    CAutomobileDoor structure
*/
#pragma once

#pragma pack(push, 1)
struct CAutomobileDoor
{
    float m_fOpenAngle;
    float m_fClosedAngle;
    short m_nDirn;
    char m_nAxis;
    char m_nDoorState;
    float m_fAngle;
    float m_fPrevAngle;
    int m_fAngVel;
};
#pragma pack(pop)