#ifndef _ITRACKER_FACTORY_H
#define _ITRACKER_FACTORY_H
#include "ITracker.h"

class ITrackerFactory
{
public:
	virtual void Init();
	virtual void Start();
    virtual ITracker *CreateTracker()=0;
};

#endif
