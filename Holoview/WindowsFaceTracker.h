#include "Factory.h"
#include "ITrackerFactory.h"
#include "KinectTrackerFactory.h"

#pragma once
class WindowsFaceTracker :  public KinectTracker, public Callable
{
public:
	WindowsFaceTracker () : KinectTracker() {};
private:
	void TrackEvent(void *message)
	{
		if (m_CallBack)
		{
			m_CallBack(m_CallBackParam);
		}
	}
};
