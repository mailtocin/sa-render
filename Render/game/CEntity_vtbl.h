/*
    SA::Render source file
    CEntity_vtbl structure
*/
#pragma once

#pragma pack(push, 1)
struct CEntity_vtbl
{
    int __dtor;
    int field_4;
    int add;
    int remove;
    int field_10;
    int setModelIndex;
    int setModelIndexNoCreate;
    int createRwObject;
    int deleteRwObject;
    int getBoundRect;
    int processControl;
    int processCollision;
    int field_30;
    int field_34;
    int teleport;
    int field_3C;
    int field_40;
    int preRender;
    int render;
    int setupLighting;
    int removeLighting;
    int flagToDestroyWhenNextProcessing;
};
#pragma pack(pop)