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
	virtual void SetTrackingResults(TrackingResults *results, void* args = NULL);
	ITracker* GetTracker() { return &tracker; };

protected:
	static void TrackerUpdateStatic(void* lpParam, void* args=NULL);
	//OpenGLWindow overrides
	virtual void resizeWindow();
	OpenGL21Renderer renderer;
	KinectTracker tracker;
	int windowWidth;
	int windowHeight;
};