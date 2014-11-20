#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _MSTRACKERFACTORY_H
#define _MSTRACKERFACTORY_H

#include "ITrackerFactory.h"
#include "TrackerFactory.h"
#include "KinectTracker.h"

class MsTrackerFactory : TrackerFactory
{
public:
    TrackerFactory* operator()() { return this; }
protected:
    virtual TrackerBase* CreateTracker()
    {
        return NULL;
    }
};
#endif