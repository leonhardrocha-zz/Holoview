#include "ITracker.h"
#include "ITrackerFactory.h"
#include "TrackerManager.h"
#include "Synchronizable.h"

#pragma once
class KinectTracker :  public ITracker, Synchronizable
{	
public:
	
	KinectTracker::KinectTracker () 
	{
		tracker.Start();
	}

	virtual void PaintEvent(void *message) 
	{	
		MSG* msg = reinterpret_cast<MSG*>(message);
		if (msg != NULL)
		{
			tracker.WndProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
		}
	}

	virtual void UpdateTracker()
	{
		synchronized(KinectTracker)
		{
			
		}
	}

private:
	TrackerManager tracker;
	float m_Pitch, m_Yaw, m_Roll;

};
