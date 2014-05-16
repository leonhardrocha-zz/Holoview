#pragma once
#include "ITracker.h"
#include "ITrackerFactory.h"
#include "TrackerManager.h"

class KinectTracker :  public ITracker
{	

public:
	
	KinectTracker::KinectTracker();
	KinectTracker::~KinectTracker();

	bool Init();
	bool Start();
	virtual void PaintEvent(void *message, TrackingArgs args=NULL);
	virtual void TrackEvent(void *message, TrackingArgs args=NULL)=0;
	virtual TrackingResults GetTrackingResults(TrackingArgs args=NULL);
	static void TrackCallback(void *param, TrackingArgs args=NULL); 
	ITracker* GetTrackerManager() { return m_pTrackerManager; } ;

private:

	ITracker* m_pTrackerManager;

};

