#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _ITRACKER_H
#define _ITRACKER_H
#include "IArgs.h"

class ITracker
{
public:
    virtual bool    Init(IArgs* args=NULL)=0;
    virtual bool    Start(IArgs* args=NULL)=0;
    virtual void    TrackEvent(IArgs* args=NULL)=0;
};

#endif