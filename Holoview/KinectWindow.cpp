#include "stdafx.h"
#include "KinectWindow.h"
#include "TrackingResults.h"

KinectWindow::KinectWindow() : KinectView(), m_pResults(NULL)
{
}

void KinectWindow::ResetWindow()
{
	SetupWindow(width(), height());
}

void KinectWindow::SetupView()
{
	if (!m_pResults)
	{
		return;
	}
}


void KinectWindow::SetupWindow(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	viewWidth = width;
	viewHeight = height;
}


void KinectWindow::initialize()
{
	Init();
	renderer.Initialize();
	SetRenderer((BaseAssetRenderer&) renderer);
	tracker.SetTrackerCallback(KinectWindow::TrackerUpdateStatic, this);
}

void KinectWindow::render()
{
	if (!m_pResults)
	{
		return;
	}
	glClear(GL_COLOR_BUFFER_BIT);
	RenderView();
}

void KinectWindow::SetupScene()
{
	if (!m_pResults)
	{
		return;
	}
	
	glMatrixMode(GL_MODELVIEW);	
	//glm::mat4 modelView = m_pResults->GetCameraView();
	//glMultMatrixf(glm::value_ptr(modelView));
	
}

void KinectWindow::RenderScene()
{
	glMatrixMode(GL_MODELVIEW);
	{		
		glPushMatrix();
		if (assetRenderer != NULL)
		{
			assetRenderer->Render();
		}
		glPopMatrix();
	}	
}

void KinectWindow::TrackerUpdateStatic(void* lpParam, TrackingArgs args)
{
	KinectWindow* pThis = reinterpret_cast<KinectWindow*>(lpParam);
	TrackingResults* results = static_cast<TrackingResults*>(args->GetArgValue("trackerId"));	
	pThis->SetTrackingResults(results);	
	pThis->render();
}

void KinectWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	render();
}