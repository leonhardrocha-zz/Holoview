#pragma once
#include "ITracker.h"
#include "OpenGLHelper.h"
#include "OpenGLWindow.h"
#include "KinectTracker.h"
#include "KinectView.h"

class KinectWindow : public OpenGLWindow, KinectView
{
public:
	KinectWindow();
	~KinectWindow() {};
	virtual void initialize();
    virtual void render();
	virtual void SetupWindow(int width, int height);
    virtual void ResetWindow();
	virtual void SetupView();
	virtual void SetupScene();
	virtual void RenderScene();
	virtual void SetTrackingResults(TrackingResults *results) { m_pResults = results; };
	ITracker* GetTracker() { return &tracker; };

protected:
	static void TrackerUpdateStatic(void* lpParam, TrackingArgs args=NULL);
	//OpenGLWindow overrides
	virtual void resizeWindow();
	OpenGL21Renderer renderer;
	KinectTracker tracker;
	TrackingResults* m_pResults;

	Pose m_3rdPersonView;
	int windowWidth;
	int windowHeight;

};