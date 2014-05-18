#include "stdafx.h"
#include "AssetWindow.h"

AssetWindow::AssetWindow() : ToeInStereoView()
{
}

void AssetWindow::ResetWindow()
{
	windowWidth = width();
	windowHeight = height();
	windowOffsetX = 0;
	windowOffsetY = 0;
}

void AssetWindow::initialize()
{
	ResetWindow();
	SetupWindow();
	ResetView();
	SetupView();
	SetupScene();
	renderer.Initialize();
	SetRenderer((BaseAssetRenderer&) renderer);
	tracker.SetTrackerCallback(AssetWindow::TrackerUpdateStatic, this);
}

void AssetWindow::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
	RenderStereoView();
}


void AssetWindow::RenderScene()
{
	renderer.Render();
}

void AssetWindow::TrackerUpdateStatic(void* lpParam, void* args)
{
	AssetWindow* pThis = reinterpret_cast<AssetWindow*>(lpParam);
	TrackingResults* results = static_cast<TrackingResults*>(args);	
	pThis->SetViewAngles(results->eulerAngles[Pitch], results->eulerAngles[Yaw], results->eulerAngles[Roll]);
	pThis->render();
}

void AssetWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	SetupScene();
}

//void AssetWindow::exposeEvent(QExposeEvent *event)
//{
//    Q_UNUSED(event);
//
//    if (isExposed())
//	{
//		//TrackerUpdateStatic(this);
//        renderNow();
//	}
//}