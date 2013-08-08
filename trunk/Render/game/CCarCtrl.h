/*
    SA::Render source file
    CCarCtrl structure
*/
#pragma once
#include "CNodeInfo.h"
#include "CVector.h"

#pragma pack(push, 1)
struct CCarCtrl
{
    CNodeInfo field_0;
    CNodeInfo startingRouteNode;
    CNodeInfo field_8;
    unsigned int dwordC;
    unsigned int dword10;
    short m_wCurrentNaviNodeInfo;
    short m_wNextPathPointId;
    short m_wPreviousPathPointId;
    char gap_1A[2];
    unsigned int time1;
    unsigned int time2;
    unsigned char f24[1];
    unsigned char byte25;
    unsigned char byte26;
    unsigned char byte27;
    unsigned char byte28;
    unsigned char m_bDrivingStyle;
    unsigned char m_bMission;
    unsigned char m_bActionId;
    int m_dwActionTime;
    unsigned int dword30;
    unsigned char byte34;
    unsigned char byte35;
    unsigned char f36[2];
    unsigned int float38;
    unsigned int dword3C;
    unsigned char byte40;
    unsigned char byte41;
    unsigned char f42[2];
    unsigned int float44;
    unsigned char f48[1];
    unsigned char byte49;
    unsigned char byte4A;
    unsigned char byte4B;
    unsigned char byte4C;
    unsigned char byte4D;
    unsigned char byte4E;
    unsigned char byte4F;
    unsigned char byte50;
    char f51;
    char gap_52[10];
    CVector m_vDestinationCoors;
    CNodeInfo m_asPathFindNodesInfo[8];
    unsigned short m_wPathFindNodesCount;
    unsigned char f8A[2];
    unsigned int dword8C;
    unsigned int dword90;
    unsigned char byte94;
    unsigned char byte95;
    short field_96;
};
#pragma pack(pop)