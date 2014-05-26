#ifndef _Inverse_TrackingResults_H
#define _Inverse_TrackingResults_H

#include "TrackingResults.h"

class InverseTrackingResults : public TrackingResults
{
public:
	InverseTrackingResults() : TrackingResults() {};


	InverseTrackingResults(const TrackingResults& parent) : TrackingResults()
	{
		m_parent = parent;	
	};
	
	virtual Pose GetAvatarPose();
	virtual void SetAvatarPose(const Pose& pose);
	protected:

	virtual void UpdateModelTransform();
	virtual void UpdateViewTransform();		
	virtual void UpdateProjectionTransform();
	virtual void UpdateModelViewTransform();	
	virtual void UpdateCameraViewTransform();
	TrackingResults m_parent;
};

#endif