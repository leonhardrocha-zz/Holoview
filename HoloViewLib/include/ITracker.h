#ifndef _ITRACKER_H
#define _ITRACKER_H
#include "IAvatar.h"
#include "IArgs.h"

class ITracker
{
public:
	virtual bool    Init()=0;
	virtual bool    Start()=0;
	virtual IArgs*  GetTrackingResults(IArgs* args=NULL)=0;
	virtual void    PaintEvent(void *message, IArgs* args=NULL)=0;
	virtual void    TrackEvent(void *message, IArgs* args=NULL)=0;
	virtual void*   GetCriticalSection()=0;
};

#endif