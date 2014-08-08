#include "vld.h"
#ifndef _IPOSE_H
#define _IPOSE_H

class IPose
{
public:
    virtual void Set(int axis, double value)=0;
    virtual double Get(int axis) = 0;
};

#endif