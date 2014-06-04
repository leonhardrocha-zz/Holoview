#include "stdafx.h"
#include "OsgWindow.h"

OsgWindow::OsgWindow() : OsgKinectView(), m_pResults(NULL)
{
}

void OsgWindow::ResetWindow()
{
	SetupWindow(width(), height());
}

void OsgWindow::SetupView()
{
	if (!m_pResults)
	{
		return;
	}
	
}


void OsgWindow::SetupWindow(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    viewWidth = width;
    viewHeight = height;
}


void OsgWindow::initialize()
{
    Init();
    scene.AddAsset("../Dependencies/Models/Collada/duck.dae");
    viewer.SetScene(scene);
    viewer.Init();
    viewer.setGeometry(0,0, viewWidth, viewHeight);
    tracker.SetTrackerCallback(OsgWindow::TrackerUpdateStatic, this);
}

void OsgWindow::render()
{
	if (!m_pResults)
	{
		return;
	}
	RenderView();
}

void OsgWindow::SetupScene()
{
}

void OsgWindow::RenderScene()
{
}

void OsgWindow::TrackerUpdateStatic(void* lpParam, void* args)
{
	OsgWindow* pThis = reinterpret_cast<OsgWindow*>(lpParam);
	TrackingResults* results = static_cast<TrackingResults*>(args);	
	pThis->SetTrackingResults(results->trackerId, results);	
	pThis->render();
}

void OsgWindow::SetTrackingResults(int trackerId, TrackingResults* results)
{	
	m_pResults = results;
}

void OsgWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	render();
}