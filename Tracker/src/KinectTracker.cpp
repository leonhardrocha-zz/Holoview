#include "stdafx.h"
#include "KinectTracker.h"

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

bool KinectTracker::Init()
{
	if(m_pTrackerManager)
	{
		m_pTrackerManager->SetTrackerCallback(TrackCallback, (void*)this, NULL);
		return m_pTrackerManager->Init();
	}
		
	return false;
}

bool KinectTracker::Start()
{
	if(m_pTrackerManager)
	{			
		return m_pTrackerManager->Start();
	}
		
	return false;
}
	
void KinectTracker::PaintEvent(void *message, TrackingArgs args) 
{	
	m_pTrackerManager->PaintEvent(message, args);
}


TrackingResults KinectTracker::GetTrackingResults(TrackingArgs args)
{
	ITracker* pTrackerManager = GetTrackerManager();
	return pTrackerManager->GetTrackingResults(args);
}

void KinectTracker::TrackCallback(void *param, TrackingArgs args) 
{	
	KinectTracker* pThis = reinterpret_cast<KinectTracker*>(param);				
	pThis->TrackEvent(pThis->m_CallBackParam);
}

