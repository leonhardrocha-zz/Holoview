#pragma once
#include "Factory.h"
#include "ITrackerFactory.h"
#include "KinectTrackerFactory.h"
#include "KinectTracker.h"


class WindowsFaceTracker :  public KinectTracker
{
public:
	WindowsFaceTracker () {};
private:
	void TrackEvent(void *message, void* args=NULL)
	{
		if (m_CallBack)
		{
			(*m_CallBack)(m_CallBackParam, NULL);
		}
	}
};

