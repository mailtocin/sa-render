/*
    SA::Render source file
    CControllerState structure
*/
#pragma once

struct CControllerState
{
    signed short LeftStickX;
    signed short LeftStickY;
    signed short RightStickX;
    signed short RightStickY;

    signed short LeftShoulder1;
    signed short LeftShoulder2;
    signed short RightShoulder1;
    signed short RightShoulder2;

    signed short DPadUp;
    signed short DPadDown;
    signed short DPadLeft;
    signed short DPadRight;

    signed short Start;
    signed short Select;

    signed short ButtonSquare;
    signed short ButtonTriangle;
    signed short ButtonCross;
    signed short ButtonCircle;

    signed short ShockButtonL;
    signed short ShockButtonR;

    signed short m_bChatIndicated;
    signed short m_bPedWalk;
    signed short m_bVehicleMouseLook;
    signed short m_bRadioTrackSkip;
};
