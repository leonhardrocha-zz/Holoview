#ifndef _ITRACKER_H
#define _ITRACKER_H
#include "Callable.h"
#include "IAvatar.h"

typedef AvatarPose TrackingResults;

class ITracker : public Callable
{
public:
	virtual bool Init()=0;
	virtual bool Start()=0;
	virtual TrackingResults* GetTrackingResults(TrackingArgs args=NULL)=0;
	virtual void PaintEvent(void *message, TrackingArgs args=NULL)=0;
	virtual void TrackEvent(void *message, TrackingArgs args=NULL)=0;
};

#endif