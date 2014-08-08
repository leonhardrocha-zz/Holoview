#include "vld.h"
#ifndef _POSE_H
#define _POSE_H
#include "IPose.h"

class Pose : public IPose
{
public:
    enum FreedomDegree { Pitch = 0, Yaw, Roll, NumOfDegrees };
    virtual void Set(int axis, double value) { eulerAngle[axis] = value; };
    virtual double Get(int axis) { return eulerAngle[axis]; };

protected:
    double eulerAngle[NumOfDegrees];
};

#endif