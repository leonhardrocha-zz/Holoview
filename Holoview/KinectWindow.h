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
	virtual void SetTrackingResults(int trackedId, TrackingResults *results);
	ITracker* GetTracker() { return &tracker; };

protected:
	static void TrackerUpdateStatic(void* lpParam, void* args=NULL);
	//OpenGLWindow overrides
	virtual void resizeWindow();
	OpenGL21Renderer renderer;
	KinectTracker tracker;	
	TrackingResults* m_pResults;

	CameraPose m_3rdPersonView;
	int windowWidth;
	int windowHeight;

};