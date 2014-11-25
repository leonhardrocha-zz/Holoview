// ToeInStereoView.cpp : implementation of the StereoView class
//
#include "StereoView.h"

#define DTR 0.0174532925

struct camera
{
    GLdouble leftfrustum;
    GLdouble rightfrustum;
    GLdouble bottomfrustum;
    GLdouble topfrustum;
    GLfloat modeltranslation[3];
} leftCam, rightCam;


StereoView::StereoView()
{
	Init();
}

void StereoView::ResetWindow()
{
	windowWidth = 1024;
	windowHeight = 768;
	windowOffsetX = 0;
	windowOffsetY = 0;
	aspect = double(windowWidth)/double(windowHeight);		//screen aspect ratio	
}

void StereoView::SetupWindow()
{

}

void StereoView::Init()
{	
	fovy = 60;                                          //field of view in y-axis
	IOD = 0.06;
	CameraPosition[0] = 0;
	CameraPosition[1] = 0;
	CameraPosition[2] = 1.5;
	LookAtPosition[0] = 0;
	LookAtPosition[1] = 0;
	LookAtPosition[2] = 0.75;
	LightPosition[0] = 0;
	LightPosition[1] = 0;
	LightPosition[2] = 0;
	nearZ = 0.1;                                        //near clipping plane
	farZ = 15;                                        //far clipping plane

	ResetWindow();
	ResetLeftView();
	ResetRightView();
}


void StereoView::SetupView()
{	
	double top = nearZ*tan(DTR*fovy/2);                    //sets top of frustum based on fovy and near clipping plane
    double right = aspect*top;                             //sets right of frustum based on aspect ratio
	double frustumshift = (IOD/2)*nearZ/ abs(CameraPosition[2] - LookAtPosition[2]);

    leftCam.topfrustum = CameraPosition[1] + top;
    leftCam.bottomfrustum = CameraPosition[1] - top;
    leftCam.leftfrustum = CameraPosition[0] -right + frustumshift;
    leftCam.rightfrustum = CameraPosition[0] + right + frustumshift;
    leftCam.modeltranslation[0] = (float)IOD/2 - CameraPosition[0];
	leftCam.modeltranslation[1] = -CameraPosition[1];
	leftCam.modeltranslation[2] = -CameraPosition[2];

	rightCam.topfrustum = CameraPosition[1] + top;
    rightCam.bottomfrustum = CameraPosition[1] - top;
    rightCam.leftfrustum = CameraPosition[0] -right + frustumshift;
    rightCam.rightfrustum = CameraPosition[0] + right + frustumshift;
    rightCam.modeltranslation[0] = -(float)IOD/2 - CameraPosition[0];
	rightCam.modeltranslation[1] = -CameraPosition[1];
	rightCam.modeltranslation[2] = -CameraPosition[2];  

	glDrawBuffer(GL_BACK_LEFT);                              //draw into back left buffer
	glViewport (windowOffsetX + viewOffsetX, windowOffsetY +viewOffsetY, viewWidth/2, viewHeight);	 //sets drawing viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();                                        //reset projection matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   
}

void StereoView::SetupScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	

	float lightDif[3] = {0.8f, 0.8f, 0.8f};
	float lightAmb[3] = {0.1f, 0.1f, 0.1f};
	float lightSpec[3] = {0.1f, 0.1f, 0.1f};
	glEnable(GL_LIGHT0);		
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDif);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmb);
	glLightfv(GL_LIGHT0, GL_SPECULAR,  lightSpec);
	glLightfv(GL_LIGHT0, GL_POSITION,  LightPosition);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void StereoView::RenderLeftView()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
	{
		glFrustum(leftCam.leftfrustum, leftCam.rightfrustum,     //set left view frustum
            leftCam.bottomfrustum, leftCam.topfrustum,
            nearZ, farZ);
		//translate to cancel parallax
		glTranslatef(leftCam.modeltranslation[0],
					 leftCam.modeltranslation[1],
					 leftCam.modeltranslation[2]);
		//
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		{		
			glLightfv(GL_LIGHT0, GL_POSITION,  LightPosition);			
			RenderScene();
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void StereoView::RenderRightView()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	{		
		glFrustum(rightCam.leftfrustum, rightCam.rightfrustum,     //set left view frustum
            rightCam.bottomfrustum, rightCam.topfrustum,
            nearZ, farZ);
		//translate to cancel parallax
		glTranslatef(rightCam.modeltranslation[0],
					 rightCam.modeltranslation[1],
					 rightCam.modeltranslation[2]);
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		{
			glLightfv(GL_LIGHT0, GL_POSITION,  LightPosition);
			RenderScene();
		}
		glPopMatrix();
	}
	glPopMatrix();
}


void StereoView::RenderStereoView()						
{
	ResetLeftView();
	RenderLeftView();
	
	ResetRightView();
	RenderRightView();
}

void StereoView::ResetLeftView()
{                                    //reset modelview matrix
	viewWidth = windowWidth/2;		
	viewHeight = windowHeight;
	viewOffsetX = 0;
	viewOffsetY = 0;
	SetupView();
}

void StereoView::ResetRightView()
{
	viewWidth = windowWidth/2;		
	viewHeight = windowHeight;
	viewOffsetX = viewWidth/2;
	viewOffsetY = 0;
	SetupView();
}

void StereoView::DoOpenGLDraw()
{
	RenderStereoView();
}

void  StereoView::RenderScene()
{
	//Put render implementation on assetRenderer derived class
	if (assetRenderer != NULL)
	{
		assetRenderer->Render();
	}	
}

void StereoView::DoOpenGLResize(int nWidth, int nHeight)
{
	windowWidth = nWidth;
	windowHeight = nHeight;
	viewWidth = nWidth;
	viewHeight = nHeight;
	RenderStereoView();
}

void StereoView::SetRenderer(BaseAssetRenderer &newInstance)
{
	assetRenderer = &newInstance;
}