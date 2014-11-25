#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _POSITION_H
#define _POSITION_H
#include "IPose.h"

class Position : public IPose
{
public:
    enum Axis { XAxis = 0, YAxis, ZAxis, NumOfAxis };
    virtual inline void Set(int axis, double value) { position[axis] = value; };
    virtual inline double Get(int axis) { return position[axis]; };

protected:
    double position[NumOfAxis];
};

#endif