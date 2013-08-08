/*
    SA::Render source file
    CAiTimer structure
*/
#pragma once

#pragma pack(push, 1)
struct CAiTimer
{
    int m_nStartTime;
    int m_nInterval;
    char m_bStarted;
    char field_9;
    short __padding;
};
#pragma pack(pop)