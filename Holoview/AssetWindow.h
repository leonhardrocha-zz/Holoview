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

	float m_Pitch;
	float m_Yaw;
	float m_Roll;
	ITracker* m_pTracker;
	//OpenGLWindow overrides
	virtual void resizeWindow();
	//virtual void exposeEvent(QExposeEvent *event);
	OpenGL21Renderer renderer;

};