//------------------------------------------------------------------------------
// <copyright file="eggavatar.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"
#include "IPose.h"
#include "TrackerPose.h"
#include "AvatarFace.h"
#include <string>
#include <FaceTrackLib.h>
struct IFTImage;

class KinectTracker;

class EggAvatar
{
public:
    friend class KinectTracker;
    EggAvatar();
    ~EggAvatar();
    IPose* GetAttitude() { return static_cast<Attitude*>(&m_trackedPose); };
    IPose* GetPosition() { return static_cast<Position*>(&m_trackedPose); };
    bool SetPose(IFTResult* pResult);
protected:
    TrackerPose m_trackedPose;
    bool DrawImage(void* pImage);
    bool SetScaleAndTranslationToWindow(int height, int width);
    bool SetCandideAU(const double * AU, const int numberAU);
    bool SetRandomAU();
    bool SetRotations(const double pitchDegrees, const double yawDegrees, const double rollDegrees);
    bool SetRandomRotations();
    bool SetTranslations(const double tX, const double tY, const double tZ);
    void SetScale(double scale) { m_Scale = scale;}
    void SetTranslationX(double X){ m_TranslationX = X;}
    void SetTranslationY(double Y){ m_TranslationY = Y;}
    bool DrawBgLine(IFTImage* pImage, double x1, double y1, double x2, double y2, UINT32 color);
private:
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
