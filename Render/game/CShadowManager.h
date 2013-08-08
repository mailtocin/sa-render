/*
    SA::Render source file
    CShadowManager structure
*/
#pragma once
#include "CShadowImage.h"

#pragma pack(push, 1)
struct CShadowManager
{
    char bInitialized;
    char bNotReInitialized;
    char _pad[2];
    void *pShadowData[16];
    struct CShadowImage image;
    struct CShadowImage imageBlurred;
};
#pragma pack(pop)