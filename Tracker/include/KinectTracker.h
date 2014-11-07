#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"
#include "TrackerManager.h"

class KinectTracker :  public TrackerManager
{    

public:
    KinectTracker::KinectTracker(ITracker* parent=NULL, std::string windowHangerArg = "TrackerWindow") : TrackerManager(parent), WindowHandlerArg(windowHangerArg) {};
    std::string                 WindowHandlerArg;
protected:
    virtual void                PaintEvent(IArgs* args=NULL);
private:

};

