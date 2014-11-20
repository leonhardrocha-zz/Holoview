#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _TRACKERFACTORY_H
#define _TRACKERFACTORY_H

#include "ITrackerFactory.h"

class TrackerFactory
{
private:
    TrackerFactory() { }
public:
    ~TrackerFactory()
    {
    }
    ITracker* CreateTracker()
    {
        return _factory->CreateTracker();
    }
protected:
    ITrackerFactory* _factory;
};
#endif