#include "OpenGLHelper.h"
#include "OpenGLWindow.h"
#include "ITracker.h"
#pragma once
class AssetWindow : public OpenGLWindow, ToeInStereoView
{
public:
	AssetWindow() {};
	~AssetWindow() {};
    void initialize();
    void render();

	//BaseStereoView overrides
	virtual void SetupWindow();
	// virtual void SetupScene();
	virtual void RenderScene();

protected:

	//OpenGLWindow overrides
	virtual void resizeWindow();
	virtual void exposeEvent(QExposeEvent *event);
	OpenGL21Renderer renderer;

};