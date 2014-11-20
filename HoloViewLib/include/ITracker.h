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
    virtual bool                Init()=0;
    virtual bool                Start()=0;
    virtual bool                Stop()=0;
    virtual bool                Release()=0;
    virtual void                TrackEvent(void *message)=0;
};

#endif