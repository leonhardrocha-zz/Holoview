#pragma once
#include "ITracker.h"
#include "ITrackerFactory.h"
#include "TrackerManager.h"

class KinectTracker :  public TrackerManager
{	

public:
	KinectTracker::KinectTracker(ITracker* parent=NULL) : TrackerManager(parent) {};
protected:
private:

};

