#include "stdafx.h"
#include "AssetWindow.h"

AssetWindow::AssetWindow() : ToeInStereoView()
{
	tracker.SetTrackerCallback(AssetWindow::TrackerUpdateStatic, this);
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

void AssetWindow::TrackerUpdateStatic(void* lpParam)
{
	AssetWindow* pThis = reinterpret_cast<AssetWindow*>(lpParam);
	pThis->SetViewAngles(pThis->tracker.m_Pitch, pThis->tracker.m_Yaw, pThis->tracker.m_Roll);
	pThis->render();
}

void AssetWindow::resizeWindow()
{
	ResetWindow();
	ResetView();
	SetupScene();
}

void AssetWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
	{
		//tracker.UpdateTracker();
        renderNow();
	}
}