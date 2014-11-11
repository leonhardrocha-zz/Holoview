#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"
#include "TrackerManager.h"

class KinectTracker :  public TrackerManager
{    

public:
    KinectTracker::KinectTracker(ITracker* parent=NULL, std::string windowHanglerArg = "WindowHanglerArg") : TrackerManager(parent), WindowHandlerArg(windowHanglerArg) {};
    std::string                 WindowHandlerArg;
    virtual void                PaintEvent(IArgs* args=NULL);
    virtual void                TrackEvent(IArgs* args=NULL);
protected:
private:

};

