#pragma once
#include "Factory.h"
#include "ITrackerFactory.h"
#include "KinectTrackerFactory.h"
#include "WindowsFaceTracker.h"

class WindowsKinectFactory :  public IKinectFactory
{
public:
	virtual FACTORYDLL_API ITracker *GetTracker() { return trackerFactory.CreateTracker();};
private:
	KinectTrackerFactory<WindowsFaceTracker> trackerFactory;
};
