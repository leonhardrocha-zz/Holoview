#pragma once
#include "ITracker.h"
#include "ITrackerFactory.h"
#include "TrackerManager.h"

class KinectTracker :  public ITracker
{	

public:
	
	KinectTracker::KinectTracker() 
	{	
		m_pTrackerManager = new TrackerManager();
	};
	
	KinectTracker::~KinectTracker() 
	{
		if(m_pTrackerManager)
		{
			delete m_pTrackerManager;
		}
	};

	bool Init()
	{
		if(m_pTrackerManager)
		{
			m_pTrackerManager->SetTrackerCallback(TrackCallback, (void*)this, NULL);
			return m_pTrackerManager->Init();
		}
		
		return false;
	}

	bool Start()
	{
		if(m_pTrackerManager)
		{			
			return m_pTrackerManager->Start();
		}
		
		return false;
	}
	
	virtual void PaintEvent(void *message, int id=0) 
	{	
		m_pTrackerManager->PaintEvent(message, id);
	}

	virtual void TrackEvent(void *message, int id=0)=0;

	virtual TrackingResults GetTrackingResults(int id=0)
	{
		ITracker* pTrackerManager = GetTrackerManager();
		return pTrackerManager->GetTrackingResults();
	}

	static void TrackCallback(void *param, void* args=NULL) 
	{	
		KinectTracker* pThis = reinterpret_cast<KinectTracker*>(param);				
		pThis->TrackEvent(pThis->m_CallBackParam);
	}

	ITracker* GetTrackerManager() { return m_pTrackerManager; } ;

private:

	ITracker* m_pTrackerManager;

};

