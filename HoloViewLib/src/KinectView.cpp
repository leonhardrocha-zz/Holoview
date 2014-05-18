#include "KinectView.h"
#include <IL/il.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

void KinectView::Init()
{
	ResetWindow();
	ResetView();
	ResetScene();
}

void KinectView::ResetWindow()
{
	SetupWindow(1024, 768);
}

void KinectView::SetupWindow(int nWidth, int nHeight)
{
	windowWidth = nWidth;
	windowHeight = nHeight;
	aspect = double(windowWidth)/double(windowHeight);
}

void KinectView::ResetView()
{	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void KinectView::SetupView(int nWidth, int nHeight)
{
	viewWidth = nWidth;
	viewHeight = nHeight;
	glViewport (0, 0, viewWidth, viewHeight);
	glMatrixMode(GL_PROJECTION);	
	glLoadMatrixf(glm::value_ptr(results.Projection));
}

void KinectView::ResetScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   
}

void KinectView::SetupScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(results.ModelView));
}

void  KinectView::RenderView()
{
	ResetView();
	ResetScene();
	SetupScene();
	RenderScene();
}

void  KinectView::RenderScene()
{
	if (assetRenderer != NULL)
	{
		assetRenderer->Render();
	}	
}

void KinectView::SetRenderer(BaseAssetRenderer &newInstance)
{
	assetRenderer = &newInstance;
}