/*
    SA::Render source file
    HeliComponents structure
*/
#pragma once
#include "RenderWare.h"

#pragma pack(push, 1)
struct HeliComponents
{
    RwFrame *nullFrame;
    RwFrame *chassis;
    RwFrame *wheel_rf_dummy;
    RwFrame *wheel_rm_dummy;
    RwFrame *wheel_rb_dummy;
    RwFrame *wheel_lf_dummy;
    RwFrame *wheel_lm_dummy;
    RwFrame *wheel_lb_dummy;
    RwFrame *door_rf_dummy;
    RwFrame *door_rr_dummy;
    RwFrame *door_lf_dummy;
    RwFrame *door_lr_dummy;
    RwFrame *static_rotor;
    RwFrame *moving_rotor;
    RwFrame *static_rotor2;
    RwFrame *moving_rotor2;
    RwFrame *rudder;
    RwFrame *elevators;
    RwFrame *msc_a;
    RwFrame *misc_b;
    RwFrame *misc_c;
    RwFrame *misc_d;
};
#pragma pack(pop)