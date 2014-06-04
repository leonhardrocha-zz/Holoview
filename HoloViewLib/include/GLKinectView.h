#ifndef _GLKinectView_H_
#define _GLKinectView_H_

#include "IView.h"
#include "BaseAssetRenderer.h"
#include "TrackingResults.h"
#include <vector>
class GLKinectView : public IView
{
public:
// Attributes
public:
	GLKinectView::GLKinectView() : 
	viewWidth(1024),
	viewHeight(768)
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
	float aspect;

	BaseAssetRenderer *assetRenderer;
};

#endif