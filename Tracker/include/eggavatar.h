//------------------------------------------------------------------------------
// <copyright file="eggavatar.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#ifndef _EGGAVATAR_H
#define _EGGAVATAR_H

#include "IAvatar.h"
#include "IPose.h"
#include "TrackerPose.h"

static const double EyeInside = 1.0f/18.0f;
static const double EyeOutside = 1.0f/6.0f;
static const double EyeTop = 1.0f/40.0f;
static const double EyeBottom = -1.0f/40.0f;
static const double MouthRight = (EyeInside+EyeOutside)/2.0f;
static const double MouthLeft = -MouthRight;
static const double MouthVertical = -1.7f*(EyeOutside-EyeInside);
static const double EyebrowBottom = EyeTop + 1.0f/16.0f;
static const double EyebrowTop = EyebrowBottom + EyeTop/2.0f;
static const double EyebrowInside = EyeInside;
static const double EyebrowOutside = EyeOutside + 0.1f/16.0f;
static const double HairBottom = (0.5f+EyebrowTop)/2.0f;
static const double NoseTop = 0;
static const double NoseBottom = NoseTop - (EyeOutside-EyeInside);
static const double EyeCenterDepthCorrection = (7.0f/7.5f);
static const double PupilRadius = (3.14f/40.0f);
static const double HeadTranslationTrigger = 0.02f; // 2 centimeters
static const double HeadRotationTriggerInDegrees = 1.0; // 1 degree



static const int PointsPerEyeLid = 8;
static const int NumberEyePoints = 2*PointsPerEyeLid;
static const int NumberEyebrowPoints = 9;
static const int PointsPerLip = 8;
static const int NumberMouthPoints = 2*PointsPerLip;
static const int NumberNosePoints = 5;
static const int NumberCirclePoints = 64;
static const int PointPerSingleHair = 8;
static const int NumberOfHairs = 16;
static const int NumberFacePoints = 2*NumberEyePoints+2*NumberEyebrowPoints+NumberMouthPoints+NumberNosePoints+PointPerSingleHair*NumberOfHairs;
static const int PointsPerPupil = 32;
static const int NumberTotalPoints = NumberFacePoints + NumberCirclePoints + 2*PointsPerPupil;
static const int RightEyeFirstPoint = 0;
static const int LeftEyeFirstPoint = RightEyeFirstPoint+NumberEyePoints;
static const int RightEyebrowFirstPoint = LeftEyeFirstPoint+NumberEyePoints;
static const int LeftEyebrowFirstPoint = RightEyebrowFirstPoint+NumberEyebrowPoints;
static const int MouthFirstPoint = LeftEyebrowFirstPoint+NumberEyebrowPoints;
static const int NoseFirstPoint = MouthFirstPoint+NumberMouthPoints;
static const int HairFirstPoint = NoseFirstPoint+NumberNosePoints;
static const int RightPupilFirstPoint = NumberFacePoints;
static const int LeftPupilFirstPoint = RightPupilFirstPoint+PointsPerPupil;
static const int CircleFirstPoint = LeftPupilFirstPoint+PointsPerPupil;;

struct IFTImage;


class EggAvatar : public IAvatar
{
public:
    EggAvatar(void);
    ~EggAvatar();
    IPose* GetAttitude() { return static_cast<Attitude*>(&m_trackedPose); };
    IPose* GetPosition() { return static_cast<Position*>(&m_trackedPose); };
    bool SetCandideAU(const double * AU, const int numberAU);
    bool SetRandomAU();
    bool SetRotations(const double pitchDegrees, const double yawDegrees, const double rollDegrees);
    bool SetRandomRotations();
    bool SetTranslations(const double tX, const double tY, const double tZ);

    bool SetScaleAndTranslationToWindow(int height, int width);
    void SetScale(double scale) { m_Scale = scale;}
    void SetTranslationX(double X){ m_TranslationX = X;}
    void SetTranslationY(double Y){ m_TranslationY = Y;}

    bool DrawImage(void* pImage);
    bool DrawBgLine(IFTImage* pImage, double x1, double y1, double x2, double y2, UINT32 color);


    double m_FacePointLatLon[NumberFacePoints][2];
    double m_FacePointXYZ[NumberTotalPoints][3];

    // Weight of the different animated part in the egg avatar model
    double m_JawDrop;
    double m_UpperLipLift;
    double m_MouthStretch;
    double m_MouthCornerLift;
    double m_BrowLower;
    double m_OuterBrowRaiser;
    double m_UpperEyeLid;
    double m_LowerEyeLid;

    double m_Scale;
    double m_TranslationX;
    double m_TranslationY;
    bool m_FacingUser;
    bool m_isFirstPose;
    // Variables used for smoothing the head pose.
    bool m_HeadPositionFiltering;
    bool m_HeadRotationFiltering;
    double m_Attitude[3];
    double m_ReportedAttitude[3];
    double m_AverageAttitude[3];
    double m_TranslationSum[3];
    double m_AveragePosition[3];
    unsigned int m_SamePositionCount;
protected:
    TrackerPose m_trackedPose;
private:
    void LatLonEye(const bool left);
    void LatLonEyeBrow(const bool left);
    void LatLonMouth();
    void LatLonNose();
    void LatLonHair();
    void LatLonYaw();
    void LatLonToXYZ();
    void RollXYZ();
    void PitchXYZ();
    void CircleXYZ();
    void PupilCenter(double center[3], bool left);
    bool CanTrackPupil();
    void PupilXYZ(const bool left);
    void ScaleXYZ();
    void TranslateXYZ();
    void DrawHair(IFTImage* pImage, UINT32 color);
    void DrawPupil(IFTImage* pImage, bool left, UINT32 color);
    void DrawCurve(IFTImage* pImage, int firstPoint, int numberPoints, bool shouldClose, UINT32 color);
    void DrawSegment(IFTImage* pImage, int firstPoint, int secondPoint, UINT32 color);

    bool PointInsideCurve(double x, double y, int firstCurvePoint, int numberCurvePoints);

};
#endif