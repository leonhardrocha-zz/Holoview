#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _IPOSE_H
#define _IPOSE_H

class IPose
{
public:
    virtual void Set(int axis, double value)=0;
    virtual double Get(int axis) = 0;
};

#endif