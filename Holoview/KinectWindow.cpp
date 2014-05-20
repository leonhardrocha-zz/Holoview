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
	if (m_pResults.empty())
	{
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT);
	RenderView();
}

void KinectWindow::TrackerUpdateStatic(void* lpParam, void* args)
{
	KinectWindow* pThis = reinterpret_cast<KinectWindow*>(lpParam);
	TrackingResults* results = static_cast<TrackingResults*>(args);	
	pThis->SetTrackingResults(results);
	pThis->render();
}

void KinectWindow::SetTrackingResults(TrackingResults* results, void* args)
{
	int id = (args != NULL) ? *static_cast<int*>(args) : 0;
	auto index = m_pResults.find(id);
	if (index == m_pResults.end())
	{
		m_pResults.insert(m_pResults.end(), std::pair<int,TrackingResults*>(id, results));
	}
	m_pResults[id] = results;
	//SetupScene();
}

void KinectWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	render();
}