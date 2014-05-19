#include "stdafx.h"
#include "KinectWindow.h"

KinectWindow::KinectWindow() : KinectView()
{
}

void KinectWindow::ResetWindow()
{
	SetupWindow(width(), height());
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
	if (m_pResults)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		RenderView();
	}
}

void KinectWindow::TrackerUpdateStatic(void* lpParam, void* args)
{
	KinectWindow* pThis = reinterpret_cast<KinectWindow*>(lpParam);
	TrackingResults* results = static_cast<TrackingResults*>(args);	
	pThis->SetTrackingResults(results);
	pThis->render();
}

void KinectWindow::SetTrackingResults(TrackingResults *pResults)
{
	m_pResults = pResults;
	SetupScene();
}

void KinectWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	render();
}