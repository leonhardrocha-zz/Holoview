// DualStereoView.cpp : implementation of the DualStereoView class
//
#include "DualStereoView.h"
#include "MatrixMath.h"


DualStereoView::DualStereoView() : StereoView()
{
	
}

void DualStereoView::SetupDualStereoView()
{
	leftStereoView.windowWidth = windowWidth;
	leftStereoView.windowHeight = windowHeight;
	leftStereoView.windowOffsetX = 0;
	leftStereoView.windowOffsetY = 0;

	leftStereoView.viewWidth = viewWidth;
	leftStereoView.viewHeight = viewHeight;
	leftStereoView.viewOffsetX = 0;
	leftStereoView.viewOffsetY = 0;

	
	leftStereoView.SetupView();
		
	rightStereoView.windowWidth = windowWidth;
	rightStereoView.windowHeight = windowHeight;
	rightStereoView.windowOffsetX = windowWidth / 2;
	rightStereoView.windowOffsetY = 0;
	
	rightStereoView.viewWidth = viewWidth;
	rightStereoView.viewHeight = viewHeight;
	rightStereoView.viewOffsetX = viewWidth / 2;
	rightStereoView.viewOffsetY = 0;
	
	rightStereoView.SetupView();
}

void DualStereoView::SetupView()
{	
	SetupDualStereoView();
	leftStereoView.SetupView();
	rightStereoView.SetupView();
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