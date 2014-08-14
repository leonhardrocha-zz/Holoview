#include "vld.h"
#ifndef _ITRACKINGRESULTS_H
#define _ITRACKINGRESULTS_H

#include "IPose.h"
#include <string>

class ITrackingResults
{
public:
	virtual void SetPose(IPose* pose, std::string name = "defaultPose") = 0;
	virtual IPose* GetPose(std::string name = "defaultPose") = 0;
};

#endif