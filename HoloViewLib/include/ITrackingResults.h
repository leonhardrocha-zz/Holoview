#include "vld.h"
#ifndef _ITRACKINGRESULTS_H
#define _ITRACKINGRESULTS_H

#include "IPose.h"

class ITrackingResults
{
public:
	virtual void SetPose(IPose* pose) = 0;
	virtual IPose* GetPose() = 0;
};

#endif