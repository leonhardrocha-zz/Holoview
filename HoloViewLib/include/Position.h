#include "vld.h"
#ifndef _POSITION_H
#define _POSITION_H
#include "IPose.h"

class Position : public IPose
{
public:
    enum Axis { X = 0, Y, Z, NumOfAxis };
    virtual void Set(int axis, double value) { position[axis] = value; };
    virtual double Get(int axis) { return position[axis]; };

protected:
    double position[NumOfAxis];
};

#endif