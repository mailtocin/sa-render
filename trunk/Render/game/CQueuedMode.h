/*
    SA::Render source file
    CQueuedMode structure
*/
#pragma once

#pragma pack(push, 1)
struct CQueuedMode
{
    short Mode;
    short __padding0;
    float Duration;
    short MinZoom;
    short MaxZoom;
};
#pragma pack(pop)