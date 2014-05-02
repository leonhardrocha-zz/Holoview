#include "Factory.h"
#include "ITrackerFactory.h"
#include "KinectTrackerFactory.h"

#pragma once
class WindowsKinectFactory :  public IKinectFactory
{
public:
	virtual FACTORYDLL_API ITrackerFactory *GetTrackerFactory() { return &trackerFactory;};
private:
	KinectTrackerFactory trackerFactory;
};
