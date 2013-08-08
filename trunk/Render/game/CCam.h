/*
    SA::Render source file
    CCam structure
*/
#pragma once
#include "RenderWare.h"
#include "CEntity.h"
#include "CAutomobile.h"
#include "CPed.h"

#pragma pack(push, 1)
struct CCam
{
    unsigned char bBelowMinDist;
    unsigned char bBehindPlayerDesired;
    unsigned char m_bCamLookingAtVector;
    unsigned char m_bCollisionChecksOn;
    unsigned char m_bFixingBeta;
    unsigned char m_bTheHeightFixerVehicleIsATrain;
    unsigned char LookBehindCamWasInFront;
    unsigned char LookingBehind;
    unsigned char LookingLeft;
    unsigned char LookingRight;
    unsigned char ResetStatics;
    unsigned char Rotating;
    unsigned short Mode;
    short m_uiFinishTime;
    unsigned int m_iDoCollisionChecksOnFrameNum;
    unsigned int m_iDoCollisionCheckEveryNumOfFrames;
    unsigned int m_iFrameNumWereAt;
    unsigned int m_iRunningVectorArrayPos;
    unsigned int m_iRunningVectorCounter;
    unsigned int DirectionWasLooking;
    unsigned int f_max_role_angle;
    unsigned int f_Roll;
    unsigned int f_rollSpeed;
    unsigned int m_fSyphonModeTargetZOffSet;
    unsigned int m_fAmountFractionObscured;
    unsigned int m_fAlphaSpeedOverOneFrame;
    unsigned int m_fBetaSpeedOverOneFrame;
    unsigned int m_fBufferedTargetBeta;
    unsigned int m_fBufferedTargetOrientation;
    unsigned int m_fBufferedTargetOrientationSpeed;
    unsigned int m_fCamBufferedHeight;
    unsigned int m_fCamBufferedHeightSpeed;
    unsigned int m_fCloseInPedHeightOffset;
    unsigned int m_fCloseInPedHeightOffsetSpeed;
    unsigned int m_fCloseInCarHeightOffset;
    unsigned int m_fCloseInCarHeightOffsetSpeed;
    unsigned int m_fDimensionOfHighestNearCar;
    unsigned int m_fDistanceBeforeChanges;
    unsigned int m_fFovSpeedOverOneFrame;
    unsigned int m_fMinDistAwayFromCamWhenInterPolating;
    unsigned char m_fPedBetweenCameraHeightOffset;
    unsigned char m_fPlayerInFrontSyphonAngleOffSet;
    unsigned char m_fRadiusForDead;
    unsigned char m_fRealGroundDist;
    unsigned int m_fTargetBeta;
    unsigned int m_fTimeElapsedFloat;
    unsigned int m_fTilt;
    unsigned int m_fTiltSpeed;
    unsigned int m_fTransitionBeta;
    unsigned int m_fTrueBeta;
    unsigned int m_fTrueAlpha;
    unsigned int m_fInitialPlayerOrientation;
    float Alpha;
    float AlphaSpeed;
    float FOV;
    float FOVSpeed;
    float Beta;
    float BetaSpeed;
    float Distance;
    float DistanceSpeed;
    float CA_MIN_DISTANCE;
    float CA_MAX_DISTANCE;
    float SpeedVar;
    float m_fCameraHeightMultiplier;
    float m_fTargetZoomGroundOne;
    float m_fTargetZoomGroundTwo;
    float m_fTargetZoomGroundThree;
    float m_fTargetZoomOneZExtra;
    float m_fTargetZoomTwoZExtra;
    float m_fTargetZoomTwoInteriorZExtra;
    float m_fTargetZoomThreeZExtra;
    float m_fTargetZoomZCloseIn;
    float m_fMinRealGroundDist;
    float m_fTargetCloseInDist;
    float Beta_Targeting;
    float X_Targetting;
    float Y_Targetting;
    unsigned int CarWeAreFocussingOn;
    float CarWeAreFocussingOnI;
    float m_fCamBumpedHorz;
    float m_fCamBumpedVert;
    unsigned int m_nCamBumpedTime;
    unsigned int CAM_BUMPED_SWING_PERIOD;
    unsigned int CAM_BUMPED_END_TIME;
    float CAM_BUMPED_DAMP_RATE;
    float CAM_BUMPED_MOVE_MULT;
    RwV3D m_cvecSourceSpeedOverOneFrame;
    RwV3D m_cvecTargetSpeedOverOneFrame;
    RwV3D m_cvecUpOverOneFrame;
    RwV3D m_cvecTargetCoorsForFudgeInter;
    RwV3D m_cvecCamFixedModeVector;
    RwV3D m_cvecCamFixedModeSource;
    RwV3D m_cvecCamFixedModeUpOffSet;
    RwV3D m_vecLastAboveWaterCamPosition;
    RwV3D m_vecBufferedPlayerBodyOffset;
    RwV3D Front;
    RwV3D Source;
    RwV3D SourceBeforeLookBehind;
    RwV3D Up;
    RwV3D m_arrPreviousVectors[2];
    RwV3D m_aTargetHistoryPos[4];
    unsigned int m_nTargetHistoryTime[4];
    unsigned int m_nCurrentHistoryPoints;
    CEntity *CamTargetEntity;
    float m_fCameraDistance;
    float m_fIdealAlpha;
    float m_fPlayerVelocity;
    CAutomobile *m_pLastCarEntered;
    CPed *m_pLastPedLookedAt;
    bool m_bFirstPersonRunAboutActive;
};
#pragma pack(pop)