#include "KinectView.h"
#include <IL/il.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>



void KinectView::Init()
{
	ResetView();
	ResetScene();
}


void KinectView::ResetView()
{	
	glViewport (0, 0, viewWidth, viewHeight);
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity();
	glEnable(GL_LIGHT0);

	float lightDif[] = {0.8f, 0.8f, 0.8f, 1.0f};
	float lightAmb[] = {0.1f, 0.1f, 0.1f, 1.0f};
	float lightSpc[] = {0.1f, 0.1f, 0.1f, 1.0f};
	float lightPos[] = {0.8f, 1.2f, 0.0f, 1.0f};	

	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDif);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpc);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

}

void KinectView::SetupView()
{
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45.0f, 4.0f/3.0f, 0.01f, 10.0f);
}

void KinectView::ResetScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glShadeModel (GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void KinectView::SetupScene()
{
	glMatrixMode(GL_MODELVIEW);	
}

void  KinectView::RenderView()
{
	ResetView();	
	SetupView();
	{		
		glPushMatrix();		
		{
			ResetScene();	
			SetupScene();
			glPushMatrix();
			{				
				RenderScene();
			}
			glPopMatrix();	
		}
		glPopMatrix();	
	}
}

void  KinectView::RenderScene()
{
	glMatrixMode(GL_MODELVIEW);
	{
		glPushMatrix();
		if (assetRenderer != NULL)
		{
			assetRenderer->Render();
		}
		glPopMatrix();
	}	
}

void KinectView::SetRenderer(BaseAssetRenderer &newInstance)
{
	assetRenderer = &newInstance;
}