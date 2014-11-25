#ifdef _MSC_VER
#pragma once
#endif


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
