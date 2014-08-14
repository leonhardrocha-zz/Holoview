#pragma once
#include "ITracker.h"
#include "ITrackerFactory.h"
#include "TrackerManager.h"

class KinectTracker :  public TrackerManager
{    

public:
    KinectTracker::KinectTracker(ITracker* parent=NULL) : TrackerManager(parent) {};
    bool  InitDefault(int maxNumOfSensors = 1) { m_maxNumOfSensors = maxNumOfSensors; return Init(); };
protected:
private:
};

