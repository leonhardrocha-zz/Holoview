#pragma once
#include "IView.h"
#include "ITracker.h"
#include "OpenGLHelper.h"
#include "OpenGLWindow.h"
#include "KinectTracker.h"
#include "OsgKinectView.h"
#include "TrackingResults.h"
#include "ViewerWidget.h"

class OsgWindow : public OpenGLWindow, OsgKinectView
{
public:
	OsgWindow();
	~OsgWindow() {};
	virtual void initialize();
    virtual void render();
	virtual void SetupWindow(int width, int height);
    virtual void ResetWindow();
	virtual void SetupView();
	virtual void SetupScene();
	virtual void RenderScene();
	virtual void SetTrackingResults(int trackedId, TrackingResults *results);
	ITracker* GetTracker() { return &tracker; };
	ViewerWidget* GetViewer() { return &viewer; };

protected:
	static void TrackerUpdateStatic(void* lpParam, void* args=NULL);
	//OpenGLWindow overrides
	virtual void resizeWindow();
	ViewerWidget viewer;
    OsgScene scene;
	KinectTracker tracker;
	TrackingResults* m_pResults;

	Pose m_3rdPersonView;
	int windowWidth;
	int windowHeight;

};