#ifndef _ITRACKER_H
#define _ITRACKER_H
#include "Callable.h"

typedef void *TrackingResults;

class ITracker : public Callable
{
public:
	virtual bool Init()=0;
	virtual bool Start()=0;
	virtual TrackingResults GetTrackingResults(int id=0)=0;
	virtual void PaintEvent(void *message, int id=0)=0;
	virtual void TrackEvent(void *message, int id=0)=0;
};

#endif