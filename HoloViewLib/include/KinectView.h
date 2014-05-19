#ifndef _KINECTVIEW_H_
#define _KINECTVIEW_H_

#include "BaseAssetRenderer.h"
#include "TrackingResults.h"

class KinectView
{
public:
// Attributes
public:
	KinectView::KinectView() : 
	viewWidth(1024),
	viewHeight(768),
	m_pResults(NULL)
	{	
	}

// Operations
public:
// Overrides
	virtual void Init();
	virtual void ResetView();
	virtual void SetupView();
	virtual void ResetScene();
	virtual void SetupScene();
	virtual void RenderView();
	virtual void RenderScene();
protected:
	virtual void SetRenderer(BaseAssetRenderer &newInstance);
	// Generated message map functions
protected:
	int viewWidth;
	int viewHeight;
	float m_cameraPosition[3];
	TrackingResults *m_pResults;
	BaseAssetRenderer *assetRenderer;
};

#endif