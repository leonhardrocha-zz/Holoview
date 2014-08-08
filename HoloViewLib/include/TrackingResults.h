#ifndef _TrackingResults_H
#define _TrackingResults_H

#include "ITrackingResults.h"
#include "Pose.h"

class TrackingResults : public ITrackingResults
{
public:
    TrackingResults()  {}
    virtual void SetPose(IPose* pose) { m_pose = pose; };
    virtual IPose* GetPose() { return m_pose; };
    int TrackerId;
protected:
    IPose* m_pose;
};



#endif