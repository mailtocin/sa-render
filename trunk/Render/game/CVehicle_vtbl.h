/*
    SA::Render source file
    CVehicle_vtbl structure
*/
#pragma once

#pragma pack(push, 1)
struct CVehicle_vtbl
{
    void *m00;
    int m04;
    int add;
    int remove;
    int m10;
    int setModelIndex;
    int m18;
    int createRwObject;
    int deleteRwObject;
    int getBoundRect;
    int processControl;
    int processCollision;
    int m30;
    int m34;
    int teleport;
    int m3C;
    int m40;
    int preRender;
    int render;
    int setupLighting;
    int removeLighting;
    int flagToDestroyWhenNextProcessing;
    int processEntityCollision;
    int m5C;
    int processControlInput;
    int getComponentWorldPosition;
    int isComponentPresent;
    int m6C;
    int m70;
    int m74;
    int m78;
    int m7C;
    int m80;
    int m84;
    int m88;
    int m8C;
    int m90;
    int m94;
    int m98;
    int m9C;
    int mA0;
    int blowUp;
    int mA8;
    int mAC;
    int deflateTire;
    int mB4;
    int mB8;
    int mBC;
    int mC0;
    int mC4;
    int mC8;
    int mCC;
    int mD0;
    int getModelDistanceFromCenterToGround;
    int processCarAlarm;
    int mDC;
    int VehicleDamage;
    int isUpsideDown;
    int mE8;
    int getTowHitchPos;
    int mF0;
    int AttachTrailer;
    int DetachTrailer;
    int mFC;
    int m100;
    int m104;
};
#pragma pack(pop)