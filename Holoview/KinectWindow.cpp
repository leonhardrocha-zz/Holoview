#include "stdafx.h"
#include "KinectWindow.h"

KinectWindow::KinectWindow() : KinectView()
{
}

void KinectWindow::ResetWindow()
{
	SetupWindow(width(), height());
}

void KinectWindow::ResetView()
{
	SetupView(width(), height());
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

void KinectWindow::SetTrackingResults(TrackingResults *pResults)
{
	results.avatar.SetAvatarPose(pResults->GetAvatarPose());
	results.camera.SetCameraPose(pResults->GetCameraPose());
	results.UpdateTransforms();
}

void KinectWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	SetupScene();
}