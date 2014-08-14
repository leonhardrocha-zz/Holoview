#include "stdafx.h"
#ifndef _TRACKINGRESULTS_H
#define _TRACKINGRESULTS_H

#include "ITrackingResults.h"
#include "Pose.h"
#include <string>
#include <map>

class TrackingResults : public ITrackingResults
{
public:
    TrackingResults() {}
    virtual void SetPose(IPose* pose, std::string name = "defaultPose") { m_pose[name] = pose; };
    virtual IPose* GetPose(std::string name = "defaultPose") { return m_pose[name]; };
    int TrackerId;
protected:
    std::map<std::string, IPose*> m_pose;
};
#endif