#ifndef _ITRACKER_FACTORY_H
#define _ITRACKER_FACTORY_H
#include "ITracker.h"

class ITrackerFactory
{
public:
    virtual ITracker *CreateTracker()=0;
};

#endif
