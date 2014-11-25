// DualStereoView.cpp : implementation of the DualStereoView class
//
#include "DualStereoView.h"
#include "MatrixMath.h"


DualStereoView::DualStereoView() : StereoView()
{
	
}

void DualStereoView::ResetView()
{
	viewWidth = windowWidth;
	viewHeight = windowHeight;
	viewOffsetX = 0;
	viewOffsetY = 0;

	ResetLeftView();
	ResetRightView();
}


void DualStereoView::ResetLeftView()
{
	leftStereoView.windowWidth = windowWidth;
	leftStereoView.windowHeight = windowHeight;
	leftStereoView.windowOffsetX = 0;
	leftStereoView.windowOffsetY = 0;
}


void DualStereoView::ResetRightView()
{
	rightStereoView.windowWidth = windowWidth;
	rightStereoView.windowHeight = windowHeight;
	rightStereoView.windowOffsetX = windowWidth/2;
	rightStereoView.windowOffsetY = 0;
}

void DualStereoView::SetupView()
{	
	leftStereoView.SetupView();
	rightStereoView.SetupView();
}


void DualStereoView::ResetWindow()
{
	rightStereoView.ResetWindow();
	leftStereoView.ResetWindow();
}

void DualStereoView::SetupWindow()
{
	rightStereoView.SetupWindow();
	leftStereoView.SetupWindow();
}

void DualStereoView::SetupScene()
{
	rightStereoView.SetupScene();
	leftStereoView.SetupScene();
}

void DualStereoView::RenderLeftView()
{
	leftStereoView.RenderStereoView();	
}

void DualStereoView::RenderRightView()
{
	rightStereoView.RenderStereoView();
}

void DualStereoView::RenderStereoView()
{
	glDrawBuffer(GL_BACK_LEFT);                              //draw into back left buffer
	glPushMatrix();
	{
		ResetLeftView();
		RenderLeftView();
	}
	glPopMatrix();
	
	glDrawBuffer(GL_BACK_RIGHT);                             //draw into back right buffer
	glPushMatrix();
	{
		ResetRightView();
		RenderRightView();
	}
}

void DualStereoView::DoOpenGLResize(int nWidth, int nHeight)
{
	rightStereoView.DoOpenGLResize(nWidth, nHeight);
	leftStereoView.DoOpenGLResize(nWidth, nHeight);
}

void DualStereoView::SetRenderer(BaseAssetRenderer &newInstance)
{
	rightStereoView.SetRenderer(newInstance);
	leftStereoView.SetRenderer(newInstance);
}