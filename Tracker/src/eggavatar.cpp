//------------------------------------------------------------------------------
// <copyright file="eggavatar.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "StdAfx.h"

#include <FaceTrackLib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "TrackerPose.h"
#include "Position.h"

const double EyeCurveTop[PointsPerEyeLid] = {0, 0.5f, 0.775f, 0.925f, 1.0f, 0.925f, 0.775f, 0.5f};
const double EyeCurveBottom[PointsPerEyeLid] = {0, 0.5f, 0.775f, 0.925f, 1.0f, 0.925f, 0.775f, 0.5f};
const double EyebrowCurve[NumberEyebrowPoints] = {0, 0.5f, 0.775f, 0.925f, 1.0f, 0.925f, 0.775f, 0.5f, 0};
const double UpperLipCurve[PointsPerLip] = {0, 0.5f, 0.775f, 0.925f, 1.0f, 0.925f, 0.775f, 0.5f};
const double LowerLipCurve[PointsPerLip] = {0, 0.5f, 0.775f, 0.925f, 1.0f, 0.925f, 0.775f, 0.5f};
const double LipCornersCurve[NumberMouthPoints] = 
    {1.0f, 0.5f, 0.2f, 0, 0, 0, 0.2f, 0.5f, 1.0f, 0.5f, 0.2f, 0, 0, 0, 0.2f, 0.5f};
const double LipStretchCurve[NumberMouthPoints] = 
    {-1.0f, -0.925f, -0.775f, -0.5f, 0, 0.5f, 0.775f, 0.925f, 1.0f, 0.925f, 0.775f, 0.5f, 0, -0.5f, -0.775f, -0.925f};
const double OuterBrowRaiseCurveX[NumberEyebrowPoints] = 
    {0, 0.0125f, 0.025f, 0.0375f, 0.05f, 0.0625f, 0.075f, 0.0875f, 0.1f};
const double OuterBrowRaiseCurveY[NumberEyebrowPoints] = 
    {0, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f, 1.0f};


const double AU0LipLiftCoefficient = 0.5f/16.0f;
const double AU1JawDropCoefficient = 1.0f/12.0f;
const double AU2LipStretchCoefficient = 1.0f/24.0f;
const double AU3EyebrowLowerCoefficient = 2.0f/16.0f;
const double AU4MouthCornerCoefficient = -1.0f/24.0f;
const double AU5OuterBrowRaiserCoefficient = 1.0f/16.0f;

const double AU2LowerEyelidCoefficient = 1.0f/40.0f;
const double AU3EyelidsCoefficient = 2.0f/40.0f;



EggAvatar::EggAvatar()
{
    memset(m_FacePointLatLon, 0, sizeof(m_FacePointLatLon));
    memset(m_FacePointXYZ, 0, sizeof(m_FacePointXYZ));
    m_JawDrop = 0;
    m_UpperLipLift = 0;
    m_MouthStretch = 0;
    m_MouthCornerLift = 0;
    m_BrowLower = 0;
    m_OuterBrowRaiser = 0;
    m_UpperEyeLid = 0;
    m_LowerEyeLid = 0;
    m_Scale = 2;
    m_TranslationX = 0;
    m_TranslationY = 0;
    m_FacingUser = true;
    m_isFirstPose = true;
    m_HeadPositionFiltering = true;
    m_HeadRotationFiltering = false;
    m_Attitude[0] = 0;
    m_Attitude[1] = 0;
    m_Attitude[2] = 0;
    m_AverageAttitude[0] = 0;
    m_AverageAttitude[1] = 0;
    m_AverageAttitude[2] = 0;
    m_ReportedAttitude[0] = 0;
    m_ReportedAttitude[1] = 0;
    m_ReportedAttitude[2] = 0;
    m_AveragePosition[0] = 0;
    m_AveragePosition[1] = 0;
    m_AveragePosition[2] = 0;
    m_TranslationSum[0] = 0;
    m_TranslationSum[1] = 0;
    m_TranslationSum[2] = 0;
    m_SamePositionCount = 0;
}

EggAvatar::~EggAvatar()
{
}

bool EggAvatar::SetRandomAU()
{
    double AU[6];
    for (int i=0; i<6; i++)
    {
        double r = (double)(rand() & 1023);
        r /= 1023;
        r *= 2;
        r -= 1;
        AU[i] = r;
    }
    return(SetCandideAU(AU, 6));
}

bool EggAvatar::SetCandideAU(const double* AU, const int numberAU)
{
    bool ret = (numberAU >= 6);
    if (ret)
    {
        // Apply static coefficients to matching AU
        m_UpperLipLift = max(AU[0],0)*AU0LipLiftCoefficient;
        m_JawDrop = max(AU[1],0)*AU1JawDropCoefficient;
        m_MouthStretch = AU[2]*AU2LipStretchCoefficient;
        m_BrowLower = AU[3]*AU3EyebrowLowerCoefficient;
        m_MouthCornerLift = AU[4]*AU4MouthCornerCoefficient;
        m_OuterBrowRaiser =AU[5]*AU5OuterBrowRaiserCoefficient;
        m_FacingUser = true;
        // Draw the eyelids based on the other AU
        if (numberAU < 8 || (AU[6] == 0 && AU[7] == 0))
        {
            if (AU[3] > 0.1f && AU[5] > 0.05f)
            {   // If the eyebrows are lowered, draw angry eyes
                m_LowerEyeLid = AU[3]*AU3EyelidsCoefficient;
                m_UpperEyeLid = 0;
            }
            else if (AU[3] < -0.1f && AU[2] > 0.1f && AU[4] > 0.1f)
            {   // If eyebrow up and mouth stretched, draw fearful eyes
                m_LowerEyeLid = AU[3] * AU3EyelidsCoefficient;
                m_UpperEyeLid = -AU[3] * AU3EyelidsCoefficient;
            }
            else if (AU[1] > 0.1f && AU[3] < -0.1f)
            {   // if eyebrow up and mouth open, draw big surprised eyes
                m_LowerEyeLid = AU[3] * AU3EyelidsCoefficient;
                m_UpperEyeLid = -AU[3] * AU3EyelidsCoefficient;
            }
            else if ((AU[2] - AU[4]) > 0.1f && AU[4] < 0)
            {   // If lips are stretched, assume smile and draw smily eyes
                m_LowerEyeLid = min(AU[2] - AU[4], 1.0f) * AU2LowerEyelidCoefficient;
                m_UpperEyeLid = 0;
            } 
            else if ((AU[2] - AU[4]) < 0 &&  AU[5] < -0.3f)
            {   // If lips low and eyebrow slanted up draw sad eyes
                m_LowerEyeLid = 0;
                m_UpperEyeLid = -AU[4] * AU2LowerEyelidCoefficient;
            }
            else // by default, just draw the default eyes
            {
                m_LowerEyeLid = 0;
                m_UpperEyeLid = 0;
            }
        }
        else // If we capture more data on the eyes, just use it.
        {
            m_LowerEyeLid = AU[7];
            m_UpperEyeLid = AU[6];
        }
        // Check that the eyelids are not excessive!
        if (m_LowerEyeLid > -EyeBottom)
            m_LowerEyeLid = -EyeBottom;
        else if (m_LowerEyeLid < EyeBottom)
            m_LowerEyeLid = EyeBottom;
        if (m_UpperEyeLid > EyeTop)
            m_UpperEyeLid = EyeTop;
        else if (m_UpperEyeLid < -EyeTop)
            m_UpperEyeLid = -EyeTop;

        // Check that the eyebrows don't get in the eyes.
        if (m_BrowLower - m_OuterBrowRaiser > EyebrowBottom * 0.9f)
        {
            double alpha = 0.9f * EyebrowBottom / (m_BrowLower - m_OuterBrowRaiser);
            m_BrowLower *= alpha;
            m_OuterBrowRaiser *= alpha;
        }
    }
    return ret;
}

/* When setting the rotations, we use low pass filtering 
 * to compute an average "reference position." The avatar's attitude is
 * offset by that position so that the average correspond to neutral.
 */

bool EggAvatar::SetRotations(const double pitchDegrees, const double yawDegrees, const double rollDegrees)
{
    if (m_HeadRotationFiltering)
    {
        if (abs(pitchDegrees - m_ReportedAttitude[0]) < HeadRotationTriggerInDegrees || m_isFirstPose)
        {
            m_ReportedAttitude[0] = pitchDegrees;
        }
        if (abs(yawDegrees - m_ReportedAttitude[1]) < HeadRotationTriggerInDegrees || m_isFirstPose)
        {
            m_ReportedAttitude[1] = yawDegrees;
        }
        if (abs(rollDegrees - m_ReportedAttitude[2]) < HeadRotationTriggerInDegrees || m_isFirstPose)
        {
            m_ReportedAttitude[2] = rollDegrees;
        }
        m_isFirstPose = false;
    } else {
        m_ReportedAttitude[0] = pitchDegrees;
        m_ReportedAttitude[1] = yawDegrees;
        m_ReportedAttitude[2] = rollDegrees;
    }
    
    m_Attitude[0] = m_ReportedAttitude[0]/180.0;
    m_Attitude[1] = -m_ReportedAttitude[1]/180.0;
    m_Attitude[2] = m_ReportedAttitude[2]/180.0;

    m_FacingUser = (abs(m_Attitude[0]) < 0.2f && abs(m_Attitude[1]) < 0.2f);
    m_trackedPose.SetAttitude(m_Attitude);
    return TRUE;
}



// We keep track of the translation as a way to check whether the
// subject head's pose should be filtered. If the user moved
// significantly, then the pose should probably be reset.
bool EggAvatar::SetTranslations(const double tX, const double tY, const double tZ)
{
    if (m_HeadPositionFiltering)
    {
        m_SamePositionCount++;
        m_TranslationSum[0] += tX;
        m_TranslationSum[1] += tY;
        m_TranslationSum[2] += tZ;
        m_AveragePosition[0] = m_TranslationSum[0] / (double)m_SamePositionCount;
        m_AveragePosition[1] = m_TranslationSum[1] / (double)m_SamePositionCount;
        m_AveragePosition[2] = m_TranslationSum[2] / (double)m_SamePositionCount;
        double deltaMaxTxyz = max(abs(m_AveragePosition[0] - tX), abs(m_AveragePosition[1] - tY));
        deltaMaxTxyz = max(deltaMaxTxyz, abs(m_AveragePosition[2]-tZ));
        if (deltaMaxTxyz > HeadTranslationTrigger)
        {
            // do not discard translation peak
            m_SamePositionCount = 1;
            m_TranslationSum[0] = tX;
            m_TranslationSum[1] = tY;
            m_TranslationSum[2] = tZ;
            m_AveragePosition[0] = tX;
            m_AveragePosition[1] = tY;
            m_AveragePosition[2] = tZ;
        }
    } else
    {
        m_AveragePosition[0] = tX;
        m_AveragePosition[1] = tY;
        m_AveragePosition[2] = tZ;
    }
    m_trackedPose.SetPosition(m_AveragePosition);
    return TRUE;
}



bool EggAvatar::SetRandomRotations()
{
    double pitchDegrees = ((((double)(rand() & 1023)) / 1023) - 0.5f) * 90.0f;
    double yawDegrees = ((((double)(rand() & 1023)) / 1023) - 0.5f) * 90.0f;
    double rollDegrees = ((((double)(rand() & 1023)) / 1023) - 0.5f) * 90.0f;
    SetRotations(pitchDegrees, yawDegrees, rollDegrees);
    return(TRUE);
}


bool EggAvatar::SetScaleAndTranslationToWindow(int height, int width)
{
    m_Scale = ((double)min(height, width))/4;
    m_TranslationX = ((double) width)/2;
    m_TranslationY = ((double) height)/2;
    return(TRUE);
}

bool EggAvatar::DrawImage(void* image)
{
    IFTImage* pImage = static_cast<IFTImage*>(image);
    // Initialize internal points to mean shape plus animation units
    LatLonEye(false);
    LatLonEye(true);
    LatLonEyeBrow(false);
    LatLonEyeBrow(true);
    LatLonMouth();
    LatLonNose();
    LatLonHair();
    // Apply yaw to the feature points
    LatLonYaw();
    // Convert to XYZ, apply pitch, roll, draw circle, scale and translate
    LatLonToXYZ();
    PitchXYZ();
    RollXYZ();
    bool wasFacing = m_FacingUser;
    if (m_FacingUser && !CanTrackPupil())
    {
        m_FacingUser = false;
    }
    PupilXYZ(false);
    PupilXYZ(true);
    m_FacingUser = wasFacing;
    CircleXYZ();
    ScaleXYZ();
    TranslateXYZ();
    // Draw the components
    UINT32 white=0xFFFFFFFF;

    DrawCurve(pImage, RightEyeFirstPoint, NumberEyePoints, true, white);
    DrawCurve(pImage, LeftEyeFirstPoint, NumberEyePoints, true, white);
    DrawCurve(pImage, RightEyebrowFirstPoint, NumberEyebrowPoints, false, white);
    DrawCurve(pImage, LeftEyebrowFirstPoint, NumberEyebrowPoints, false, white);
    DrawCurve(pImage, MouthFirstPoint, NumberMouthPoints, true, white);
    DrawCurve(pImage, NoseFirstPoint, NumberNosePoints, false, white);
    DrawHair(pImage, white);
    DrawCurve(pImage, CircleFirstPoint, NumberCirclePoints, true, white);
    DrawPupil(pImage, true, white);
    DrawPupil(pImage, false, white);

    return TRUE;
}

// set the latitude and longitude of the eye lid points.
// if left is set, the longitude are the opposite value of right.
void EggAvatar::LatLonEye(const bool left)
{
    int firstPoint = left ? LeftEyeFirstPoint : RightEyeFirstPoint;
    double sign = left ? -1.0f : 1.0f;
    for (int i = 0; i < PointsPerEyeLid; ++i)
    {
        m_FacePointLatLon[firstPoint + i][0] = sign * (((i * (EyeOutside - EyeInside)) / PointsPerEyeLid) + EyeInside);
        m_FacePointLatLon[firstPoint + i][1] = (EyeTop + m_UpperEyeLid) * EyeCurveTop[i];
    }
    firstPoint += PointsPerEyeLid;
    for (int i = 0; i < PointsPerEyeLid; ++i)
    {
        m_FacePointLatLon[firstPoint + i][0] = sign * (((i * (EyeInside - EyeOutside)) / PointsPerEyeLid) + EyeOutside);
        m_FacePointLatLon[firstPoint + i][1] = (EyeBottom + m_LowerEyeLid) * EyeCurveBottom[i];
    }
}

// set the latitude and longitude of the eye lid points.
// if left is set, the longitude are the opposite value of right.
void EggAvatar::LatLonEyeBrow(const bool left)
{
    int firstPoint = left ? LeftEyebrowFirstPoint : RightEyebrowFirstPoint;
    double sign = left ? -1.0f : 1.0f;
    for (int i=0; i<NumberEyebrowPoints; ++i)
    {
        m_FacePointLatLon[firstPoint + i][0] = sign * (((i * (EyebrowOutside - EyebrowInside)) / (NumberEyebrowPoints - 1) + 
            m_OuterBrowRaiser * OuterBrowRaiseCurveX[i]) + EyebrowInside);
        m_FacePointLatLon[firstPoint + i][1] = (EyebrowTop - EyebrowBottom) * EyebrowCurve[i] + EyebrowBottom - m_BrowLower
            + m_OuterBrowRaiser * OuterBrowRaiseCurveY[i];
    }
}

// set the latitude and longitude of the eye lid points.
void EggAvatar::LatLonMouth()
{
    int firstPoint = MouthFirstPoint;
    for (int i = 0; i<PointsPerLip; ++i)
    {
        m_FacePointLatLon[firstPoint + i][0] = ((i*(MouthRight - MouthLeft)) / PointsPerLip) + MouthLeft +
            m_MouthStretch * LipStretchCurve[i];
        m_FacePointLatLon[firstPoint + i][1] = MouthVertical + m_UpperLipLift * UpperLipCurve[i] +
            m_MouthCornerLift * LipCornersCurve[i];
    }
    firstPoint += PointsPerLip;
    for (int i = 0; i<PointsPerLip; ++i)
    {
        m_FacePointLatLon[firstPoint + i][0] = ((i*(MouthLeft - MouthRight)) / PointsPerLip) + MouthRight +
            m_MouthStretch * LipStretchCurve[i + PointsPerLip];
        m_FacePointLatLon[firstPoint + i][1] = MouthVertical - m_JawDrop * LowerLipCurve[i] +
            m_MouthCornerLift * LipCornersCurve[i + PointsPerLip];
    }
}

void EggAvatar::LatLonNose()
{
    for (int i = 0; i < NumberNosePoints; ++i)
    {
        m_FacePointLatLon[NoseFirstPoint + i][0] = 0;
        m_FacePointLatLon[NoseFirstPoint + i][1] = (i * (NoseTop - NoseBottom)) / (NumberNosePoints - 1) + NoseBottom;
    }
}

void EggAvatar::LatLonHair()
{
    // For each hair
    for (int i = 0; i < NumberOfHairs; ++i)
    {
        double lon = 2.0f / NumberOfHairs;
        lon *= i + 0.5f;
        lon -= 1.0f;

        double deltaLat = 0.5f - HairBottom;
        deltaLat /= (PointPerSingleHair-1);
        int firstPoint = HairFirstPoint + i * PointPerSingleHair;

        for (int j = 0; j < PointPerSingleHair; ++j)
        {
            m_FacePointLatLon[firstPoint+j][0] = lon;
            m_FacePointLatLon[firstPoint+j][1] =  HairBottom + deltaLat * j;
        }
    }
}

void EggAvatar::LatLonYaw()
{
    static int nbHairDraw = 0;
    for (int i = 0; i < NumberFacePoints; ++i)
    {
        m_FacePointLatLon[i][0] += m_Attitude[1];
        if (m_FacePointLatLon[i][0] > 1.0f)
            m_FacePointLatLon[i][0] = m_FacePointLatLon[i][0] - 2.0f;
        if (m_FacePointLatLon[i][0] < -1.0f)
            m_FacePointLatLon[i][0] = m_FacePointLatLon[i][0] + 2.0f;
    }
}

void EggAvatar::LatLonToXYZ()
{
    for (int i = 0; i < NumberFacePoints; ++i)
    {
        double lonRadian = m_FacePointLatLon[i][0] * ((double)M_PI);
        double latRadian = m_FacePointLatLon[i][1] * ((double)M_PI);

        m_FacePointXYZ[i][1] = sin(latRadian);
        double radius = cos(latRadian);
        m_FacePointXYZ[i][0] = radius*sin(lonRadian);
        m_FacePointXYZ[i][2] = radius*cos(lonRadian);
    }
}

void EggAvatar::PitchXYZ()
{
    double pitchRadian = m_Attitude[0] * ((double)M_PI);
    double a = cos(pitchRadian);
    double b = sin(pitchRadian);
    for (int i = 0; i < NumberFacePoints; ++i)
    {
        double z = a*m_FacePointXYZ[i][2] - b * m_FacePointXYZ[i][1];
        double y = b*m_FacePointXYZ[i][2] + a * m_FacePointXYZ[i][1];

        m_FacePointXYZ[i][1] = y;
        m_FacePointXYZ[i][2] = z;
    }
}

void EggAvatar::RollXYZ()
{
    double rollRadian = m_Attitude[2] * ((double)M_PI);
    double a = cos(rollRadian);
    double b = sin(rollRadian);
    for (int i = 0; i < NumberFacePoints; ++i)
    {
        double x = a*m_FacePointXYZ[i][0] - b * m_FacePointXYZ[i][1];
        double y = b*m_FacePointXYZ[i][0] + a * m_FacePointXYZ[i][1];

        m_FacePointXYZ[i][0] = x;
        m_FacePointXYZ[i][1] = y;
    }
}


void EggAvatar::PupilCenter(double * center, bool left)
{
    int firstEyePoint = left ? LeftEyeFirstPoint : RightEyeFirstPoint;
    // Find the middle point of the eye
    double r2 = 0;

    for (int k = 0; k < 3; ++k)
    {
        center[k] = m_FacePointXYZ[firstEyePoint][k] + m_FacePointXYZ[firstEyePoint + PointsPerEyeLid][k];
        r2 += center[k] * center[k];
    }
    if (r2 > 0)
    {
        double alpha = 1 / sqrt(r2);
        for (int k = 0; k < 3; ++k)
        {
            center[k] *= alpha;
        }
    }
    // If tracking, find the projection of the center of the eye to the sphere.
    if (m_FacingUser)
    {
        center[0] *= EyeCenterDepthCorrection;
        center[1] *= EyeCenterDepthCorrection;
        center[2] = sqrt(1.0f - center[0]*center[0] - center[1]*center[1]);
    }
}

bool EggAvatar::CanTrackPupil()
{
    bool inside = m_FacingUser;
    for (int i = 0; i<2 && inside; ++i)
    {
        double center[3];
        PupilCenter(center, i==0);
        inside &= PointInsideCurve(center[0], center[1], i == 0 ? LeftEyeFirstPoint : RightEyeFirstPoint, NumberEyePoints);
    }
    return inside;
}

void EggAvatar::PupilXYZ(const bool left)
{
    int firstPupilPoint = left ? LeftPupilFirstPoint : RightPupilFirstPoint;
    // Find the middle point of the eyes
    double center[3];
    PupilCenter(center, left);

    // Draw the pupil around the selected point. 
    // First, get two orthogonal vectors
    double v1[3];
    double v2[3];
    if (center[1] > 0.1f || center[2] > 0.1f)
    {
        v1[0] = 0;
        v1[1] = center[2];
        v1[2] = -center[1];
    }
    else
    {
        v1[0] = -center[2];
        v1[1] = 0;
        v1[2] = center[0];
    }
    v2[0] = center[1] * v1[2] - center[2] * v1[1];
    v2[1] = center[2] * v1[0] - center[0] * v1[2];
    v2[2] = center[0] * v1[1] - center[1] * v1[0];
    // Scale the two vectors to the pupil's radius.
    double v1v1 = 0, v2v2 = 0;
    for (int k = 0; k < 3; ++k)
    {
        v1v1 += v1[k] * v1[k];
        v2v2 += v2[k] * v2[k];
    }
    double alpha1 = PupilRadius / sqrt(v1v1);
    double alpha2 = PupilRadius / sqrt(v2v2);
    for (int i = 0; i < 3; ++i)
    {
        v1[i] *= alpha1;
        v2[i] *= alpha2;
    }
    // Draw the pupil and normalize its position.
    for (int i = 0; i < PointsPerPupil; ++i)
    {
        double alpha = 2 * i * ((double)M_PI) / PointsPerPupil;
        double a = cos(alpha);
        double b = sin(alpha);
        for (int k = 0; k < 3; ++k)
        {
            m_FacePointXYZ[i + firstPupilPoint][k] = center[k] + a * v1[k] + b * v2[k];
        }
    }
}
void EggAvatar::DrawHair(IFTImage* pImage, UINT32 color)
{
    // For each hair
    for (int i = 0; i < NumberOfHairs; ++i)
    {
        int firstPoint = HairFirstPoint + i * PointPerSingleHair;
        DrawCurve(pImage, firstPoint, PointPerSingleHair, false, color);
    }
}

void EggAvatar::DrawPupil(IFTImage* pImage, bool left, UINT32 color)
{
    int firstEyePoint = left ? LeftEyeFirstPoint : RightEyeFirstPoint;
    int firstPupilPoint = left ? LeftPupilFirstPoint : RightPupilFirstPoint;

    bool pointIn[PointsPerPupil];
    for (int i = 0; i < PointsPerPupil; ++i)
    {
        pointIn[i] = PointInsideCurve(m_FacePointXYZ[i + firstPupilPoint][0], 
            m_FacePointXYZ[i + firstPupilPoint][1], firstEyePoint, NumberEyePoints);
    }

    for (int i1 = 0; i1 < PointsPerPupil; ++i1)
    {
        int i2 = (i1 == 0 ? PointsPerPupil : i1) - 1;
        if (pointIn[i1] && pointIn[i2])
        {
            DrawSegment(pImage, i1 + firstPupilPoint, i2 + firstPupilPoint, color);
        }
    }
}

void EggAvatar::CircleXYZ()
{
    for (int i = 0; i < NumberCirclePoints; ++i)
    {
        double alpha = 2 * i * ((double)M_PI) / NumberCirclePoints;
        m_FacePointXYZ[i + CircleFirstPoint][0] = cos(alpha);
        m_FacePointXYZ[i + CircleFirstPoint][1] = sin(alpha);
        m_FacePointXYZ[i + CircleFirstPoint][2] = 0;
    }
}

void EggAvatar::ScaleXYZ()
{
    for (int i = 0; i < NumberTotalPoints; ++i)
    {
        m_FacePointXYZ[i][0] *= m_Scale;
        m_FacePointXYZ[i][1] *= m_Scale;
        m_FacePointXYZ[i][2] *= m_Scale;
    }
}

void EggAvatar::TranslateXYZ()
{
    for (int i = 0; i < NumberTotalPoints; ++i)
    {
        m_FacePointXYZ[i][0] += m_TranslationX;
        m_FacePointXYZ[i][1] += m_TranslationY;
    }
}

void EggAvatar::DrawCurve(IFTImage* pImage, int firstPoint, int numberPoints, bool shouldClose, UINT32 color)
{
    for (int i = 1; i < numberPoints; ++i)
    {
        DrawSegment(pImage, firstPoint + i - 1, firstPoint + i, color);
    }
    if (shouldClose)
    {
        DrawSegment(pImage, firstPoint + numberPoints - 1, firstPoint, color);
    }
}

void EggAvatar::DrawSegment(IFTImage* pImage, int firstPoint, int secondPoint, UINT32 color)
{
    double x1 = m_FacePointXYZ[firstPoint][0];
    double y1 = m_FacePointXYZ[firstPoint][1];
    double z1 = m_FacePointXYZ[firstPoint][2];
    double x2 = m_FacePointXYZ[secondPoint][0];
    double y2 = m_FacePointXYZ[secondPoint][1];
    double z2 = m_FacePointXYZ[secondPoint][2];

    if (z1 < 0)
    {
        if (z2 > 0)
        {
            double r = z2/(z2-z1);
            x1 = x2 - r*(x2-x1);
            y1 = y2 - r*(y2-y1);
        }
        else
        {
            // Segment is completely hidden
            return;
        }
    }
    else if (z2 < 0)
    {
        if (z1 > 0)
        {
            double r = z1/(z2-z1);
            x2 = x1 + r*(x1-x2);
            y2 = y1 + r*(y1-y2);
        }
        else
        {
            // Segment is completely hidden
            return;
        }
    }
    
    POINT start = {INT(x1), INT(y1)}, end = {INT(x2), INT(y2)};
    pImage->DrawLine(start, end, color, 1);
}

bool EggAvatar::PointInsideCurve(double x, double y, int firstCurvePoint, int numberCurvePoints)
{
    // First, find the segments that the points cuts in Y
    int nbCuts = 0;
    int cutIndices[2];
    double yValues[2];

    for (int i1 = 0; i1 < numberCurvePoints && nbCuts < 2; ++i1)
    {
        int i2 = (i1 == 0)?(numberCurvePoints-1):i1-1;
        if ((x >= m_FacePointXYZ[firstCurvePoint + i1][0] && x < m_FacePointXYZ[firstCurvePoint + i2][0])||
            (x < m_FacePointXYZ[firstCurvePoint + i1][0] && x >= m_FacePointXYZ[firstCurvePoint + i2][0]))
        {
            cutIndices[nbCuts] = i1;
            yValues[nbCuts] = m_FacePointXYZ[firstCurvePoint + i1][1] +
                ((m_FacePointXYZ[firstCurvePoint + i2][1] - m_FacePointXYZ[firstCurvePoint + i1][1]) *
                (x - m_FacePointXYZ[firstCurvePoint + i1][0])) / (m_FacePointXYZ[firstCurvePoint + i2][0] -  m_FacePointXYZ[firstCurvePoint + i1][0]);
            nbCuts++;
        }
    }

    return ((nbCuts == 2) && ((y >= yValues[0] && y <= yValues[1])||(y >= yValues[1] && y <= yValues[0])));
}

bool EggAvatar::DrawBgLine(IFTImage* pImage, double x1, double y1, double x2, double y2, UINT32 color)
{
    // First, get the box outline of the egg
    double cx1 = m_FacePointXYZ[CircleFirstPoint][0];
    double cy1 = m_FacePointXYZ[CircleFirstPoint][1];
    double cx2 = cx1;
    double cy2 = cy1;
    for (int i = 1; i < NumberCirclePoints; ++i)
    {
        if (cx1 > m_FacePointXYZ[CircleFirstPoint+i][0])
            cx1 = m_FacePointXYZ[CircleFirstPoint+i][0];
        else if (cx2 < m_FacePointXYZ[CircleFirstPoint+i][0])
            cx2 = m_FacePointXYZ[CircleFirstPoint+i][0];
        if (cy1 > m_FacePointXYZ[CircleFirstPoint+i][1])
            cy1 = m_FacePointXYZ[CircleFirstPoint+i][1];
        else if (cy2 < m_FacePointXYZ[CircleFirstPoint+i][1])
            cy2 = m_FacePointXYZ[CircleFirstPoint+i][1];
    }
    double cx = (cx1 + cx2)/2.0f;
    double cy = (cy1 + cy2)/2.0f;
    double r = cx1 - cx;
    double r2 = r * r;

    // Look for intersection.
    double vx = x2 - x1;
    double vy = y2 - y1;
    double vl = sqrt(vx*vx + vy*vy);
    vx /= vl;
    vy /= vl;
    double l = (x2 - x1) * vx + (y2 - y1) * vy;
    double h = (cx - x1) * vx + (cy - y1) * vy;
    double hx = x1 + vx * h;
    double hy = y1 + vy * h;
    double d2 = (hx - cx) * (hx - cx) +  (hy - cy) * (hy - cy);

    if (d2 >= r2)
    {
        // No intersection! Just draw the line
        POINT start = {INT(x1), INT(y1)}, end = {INT(x2), INT(y2)};
        pImage->DrawLine(start, end, color, 1);
    }
    else
    {
        double e = sqrt(r2 - d2);
        double h1 = h - e;
        double h2 = h + e;

        if (h1 >= l || h2 <= 0)
        {
            // no intersection
            POINT start = {INT(x1), INT(y1)}, end = {INT(x2), INT(y2)};
            pImage->DrawLine(start, end, color, 1);
        }
        else
        {
            if (h1 > 0)
            {
                // draw x1-h1
                POINT start = {INT(x1), INT(y1)}, end = {INT(x1 + h1*vx), INT(y1 + h1*vy)};
                pImage->DrawLine(start, end, color, 1);
            }
            if (h2 < l)
            {
                // draw h2-x2
                POINT start = {INT(x1 + h2*vx), INT(y1 + h2*vy)}, end = {INT(x2), INT(y2)};
                pImage->DrawLine(start, end, color, 1);
            }
        } 
    }
    return TRUE;
}
