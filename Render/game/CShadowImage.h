/*
    SA::Render source file
    CShadowImage structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct CShadowImage
{
    RwCamera *camera;
    RwTexture *texture;
};
#pragma pack(pop)