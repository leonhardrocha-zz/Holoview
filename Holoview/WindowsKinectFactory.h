#pragma once
#include "Factory.h"
#include "ITrackerFactory.h"
#include "KinectTrackerFactory.h"
#include "KinectTracker.h"

class WindowsKinectFactory :  public IKinectFactory
{
public:
	virtual FACTORYDLL_API ITracker *GetTracker() { return trackerFactory.CreateTracker();};
private:
	KinectTrackerFactory<KinectTracker> trackerFactory;
};
