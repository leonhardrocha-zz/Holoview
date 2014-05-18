#ifndef _KINECTVIEW_H_
#define _KINECTVIEW_H_

#include "BaseAssetRenderer.h"
#include "TrackingResults.h"

class KinectView
{

public:

// Attributes
public:

// Operations
public:
// Overrides
	virtual void Init();
	virtual void ResetWindow();
	virtual void SetupWindow(int nWidth, int nHeight);
	virtual void ResetView();
	virtual void SetupView(int nWidth, int nHeight);
	virtual void ResetScene();
	virtual void SetupScene();
	virtual void RenderView();
	virtual void RenderScene();
protected:

	virtual void SetRenderer(BaseAssetRenderer &newInstance);
	// Generated message map functions
protected:
	int windowWidth;
	int windowHeight;
	int viewWidth;
	int viewHeight;
	float aspect;

	TrackingResults results;

	BaseAssetRenderer *assetRenderer;
};

#endif