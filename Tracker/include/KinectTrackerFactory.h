#include "ITracker.h"
#include "ITrackerFactory.h"
#include "KinectTracker.h"

#pragma once
class KinectTrackerFactory :  public ITrackerFactory
{	
public:
	virtual ITracker *CreateTracker() { return new KinectTracker(); };

private:

};
