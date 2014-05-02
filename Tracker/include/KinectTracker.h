#include "ITracker.h"
#include "ITrackerFactory.h"
#include "SingleFace.h"

#pragma once
class KinectTracker :  public ITracker
{	
public:
	
	KinectTracker::KinectTracker () 
	{
		tracker.InitInstance();
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
		if (tracker.m_pEggAvatar)
		{
			m_Pitch = tracker.m_Pitch;
			m_Yaw = tracker.m_Yaw;
			m_Roll = tracker.m_Roll;
		}
	}

private:
	SingleFace tracker;
	float m_Pitch, m_Yaw, m_Roll;

};
