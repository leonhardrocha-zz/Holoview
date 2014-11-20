#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _ITRACKERFACTORY_H
#define _ITRACKERFACTORY_H

#include "ITracker.h"
#include "IArgs.h"
#include "Singleton.h"
#include <string>

class ITrackerFactory
{
public:
    virtual ITracker* CreateTracker()=0;
};
#endif