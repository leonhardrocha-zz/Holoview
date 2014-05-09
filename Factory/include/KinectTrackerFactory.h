#pragma once
#include "ITracker.h"
#include "ITrackerFactory.h"
#include "KinectTracker.h"

template <class T>
class KinectTrackerFactory : ITrackerFactory
{	
public:
	virtual ITracker *CreateTracker() { /* to do */ };

private:
};
