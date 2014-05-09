#include "OpenGLHelper.h"
#include "OpenGLWindow.h"
#include "KinectTracker.h"
#include "WindowsFaceTracker.h"
#pragma once
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
	ITracker* GetTracker() { return static_cast<KinectTracker*>(&tracker); };

protected:
	static void TrackerUpdateStatic(void* lpParam);
	//OpenGLWindow overrides
	virtual void resizeWindow();
	virtual void exposeEvent(QExposeEvent *event);
	OpenGL21Renderer renderer;
	WindowsFaceTracker tracker;
};