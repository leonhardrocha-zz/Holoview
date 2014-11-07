#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _ATTITUDE_H
#define _ATTITUDE_H
#include "IPose.h"

class Attitude : public IPose
{
public:
    enum FreedomDegree { Pitch = 0, Yaw, Roll, NumOfDegrees };
    virtual void Set(int axis, double value) { eulerAngle[axis] = value; };
    virtual double Get(int axis) { return eulerAngle[axis]; };

protected:
    double eulerAngle[NumOfDegrees];
};

#endif