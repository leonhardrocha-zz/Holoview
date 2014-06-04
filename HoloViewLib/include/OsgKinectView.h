#ifndef _OsgKinectView_H_
#define _OsgKinectView_H_

#include "IView.h"
#include "TrackingResults.h"
#include <vector>
class OsgKinectView : public IView
{
public:
// Attributes
public:
	OsgKinectView::OsgKinectView() : 
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
	int viewWidth;
	int viewHeight;
	float aspect;
};

#endif