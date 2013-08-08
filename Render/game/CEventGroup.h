/*
    SA::Render source file
    CEventGroup structure
*/
#pragma once

#pragma pack(push, 1)
struct CEventGroup
{
    int __vmt;
    int pPed;
    int count;
    int pEvents[16];
};
#pragma pack(pop)