#ifndef _ITRACKER_H
#define _ITRACKER_H
class ITracker
{
public:
	virtual void PaintEvent(void *message)=0;
	virtual void UpdateTracker()=0;
};
#endif