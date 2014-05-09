#ifndef _ITRACKER_H
#define _ITRACKER_H

typedef void (*FTCallBack)(void* lpParam);

class ITracker
{
public:
	virtual void PaintEvent(void *message)=0;
};

#endif