#pragma once
#include "ITracker.h"
#include "OpenGLHelper.h"
#include "OpenGLWindow.h"
#include "KinectTracker.h"

class AssetWindow : public OpenGLWindow, ToeInStereoView
{
public:
	AssetWindow();
	~AssetWindow() {};
    void initialize();
    void render();

	//BaseStereoView overrides
	virtual void ResetWindow();
	// virtual void SetupScene();
	virtual void RenderScene();
	ITracker* GetTracker() { return &tracker; };

protected:
	static void TrackerUpdateStatic(void* lpParam, TrackingArgs args=NULL);
	//OpenGLWindow overrides
	virtual void resizeWindow();
	OpenGL21Renderer renderer;
	KinectTracker tracker;
    TrackingArgs args;
};