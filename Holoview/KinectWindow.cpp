#include "stdafx.h"
#include "KinectWindow.h"
#include "TrackingResults.h"

KinectWindow::KinectWindow() : KinectView(), m_pResults(NULL)
{
	m_3rdPersonView.position.x = 0.0f;
	m_3rdPersonView.position.y = 2.0f;
	m_3rdPersonView.position.z = 0.0f;
	m_3rdPersonView.upVector.x = 0.0f;
	m_3rdPersonView.upVector.y = 1.0f;
	m_3rdPersonView.upVector.z = 0.0f;
	m_3rdPersonView.target.x = 0.8f;
	m_3rdPersonView.target.y = 1.2f;
	m_3rdPersonView.target.z = 1.5f;
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
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(m_pResults->GetProjection()));
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
	//glMultMatrixf(glm::value_ptr(glm::lookAt(m_3rdPersonView.position, m_3rdPersonView.target, m_3rdPersonView.upVector)));
	glMultMatrixf(glm::value_ptr(m_pResults->GetModelView()));
}


void KinectWindow::TrackerUpdateStatic(void* lpParam, void* args)
{
	KinectWindow* pThis = reinterpret_cast<KinectWindow*>(lpParam);
	TrackingResults* results = static_cast<TrackingResults*>(args);	
	pThis->SetTrackingResults(results->trackerId, results);
	/*AvatarPose avgPose = pThis->tracker.GetAverageCameraModel(args);*/
	CameraCoordSystem coordSys = results->GetCameraCoordSystem();
	pThis->render();
}

void KinectWindow::SetTrackingResults(int trackerId, TrackingResults* results)
{	
	m_pResults = results;
}

void KinectWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	render();
}