/*
    SA::Render source file
    CScene structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct CScene
{
    RpWorld *m_pRpWorld;
    RwCamera *m_pRwCamera;
};
#pragma pack(pop)