#pragma once
#include "ITracker.h"
#include "ITrackerFactory.h"
#include "TrackerManager.h"

class KinectTracker :  public ITracker
{	
public:
	
	KinectTracker::KinectTracker () : m_TrackCallBack(KinectTracker::TrackCallback), m_TrackCallBackParam(this)
	{
		trackerManager.SetTrackerCallback(m_TrackCallBack, m_TrackCallBackParam);
		if (trackerManager.Init())
		{
			trackerManager.Start();
		}
	}

	virtual void PaintEvent(void *message) 
	{	
		trackerManager.PaintEvent(message);
	}

	virtual void TrackEvent(void *message)=0;

	static void TrackCallback(void *param) 
	{	
		KinectTracker* pThis = static_cast<KinectTracker*>(param);
		pThis->m_Pitch = pThis->trackerManager.rotationXYZ[0];
		pThis->m_Yaw   = pThis->trackerManager.rotationXYZ[1];
		pThis->m_Roll  = pThis->trackerManager.rotationXYZ[2];	
		pThis->TrackEvent((void*)pThis->trackerManager.rotationXYZ); 
	}

	float m_Pitch, m_Yaw, m_Roll;

private:

	TrackerManager trackerManager;
	FTCallBack m_TrackCallBack;
	void* m_TrackCallBackParam;

};
