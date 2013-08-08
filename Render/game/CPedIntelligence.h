/*
    SA::Render source file
    CPedIntelligence structure
*/
#pragma once
#include "CPed.h"
#include "CPedTasks.h"
#include "CPedIntelligence__struct_34.h"
#include "CEventGroup.h"
#include "CEntityScanner.h"
#include "CAiTimer.h"
#include "CPedIntelligence__struct_18C.h"
#include "CPedIntelligence__struct_264.h"

#pragma pack(push, 1)
struct CPedIntelligence
{
    struct CPed *m_pPed;
    CPedTasks m_pedTasks;
    CPedIntelligence__struct_34 field_34;
    CEventGroup m_pedEvents;
    int m_dwDefaultDecisionMaker;
    int _DM_00;
    int m_fMeleeAccuracyA;
    int m_fMeleeAccuracyB;
    int m_dwDmNumPedsToScan;
    int m_fDmRadius;
    int field_CC;
    char field_D0;
    char eventId;
    char eventPriority;
    char field_D3;
    CEntityScanner n_vehicleScanner;
    CEntityScanner m_pedScanner;
    char field_174;
    char gap_175[3];
    CAiTimer field_178;
    int field_184;
    char field_188;
    char gap_189[3];
    CPedIntelligence__struct_18C field_18C;
    char field_260;
    char gap_261[3];
    CPedIntelligence__struct_264 field_264;
    int field_274;
    int field_278;
    char gap_27C[12];
    int field_288;
    int field_28C;
    int field_290;
};
#pragma pack(pop)