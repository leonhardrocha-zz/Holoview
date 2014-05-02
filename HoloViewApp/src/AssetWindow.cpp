#include "stdafx.h"
#include "AssetWindow.h"

AssetWindow::AssetWindow()
{
}

void AssetWindow::SetupWindow()
{
	windowWidth = width();
	windowHeight = height();
}

void AssetWindow::initialize()
{
	SetupWindow();
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

void AssetWindow::resizeWindow()
{
	SetupWindow();
	SetupView();
	SetupScene();
}

void AssetWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

	if (IsTrackering())
	{
		//m_pTracker->UpdateTracker();
	}

    if (isExposed())
        renderNow();
}