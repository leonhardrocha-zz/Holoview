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
	float lightPos[] = {0.8f, 1.2f, 2.0f, 1.0f};	

	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDif);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpc);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	m_3rdPersonView.position[Xaxis] = 0.0f;
	m_3rdPersonView.position[Yaxis] = 2.0f;
	m_3rdPersonView.position[Zaxis] = 0.0f;
	m_3rdPersonView.upVector[Xaxis] = 0.0f;
	m_3rdPersonView.upVector[Yaxis] = 1.0f;
	m_3rdPersonView.upVector[Zaxis] = 0.0f;
}

void KinectView::SetupView()
{
	if (m_pResults.empty())
	{
		return;
	}
	m_pResults[0]->UpdateTransforms();
	glMatrixMode(GL_PROJECTION);
	/*gluPerspective(45.0f, 4.0f/3.0f, 0.01f, 10.0f);*/

	glLoadMatrixf(glm::value_ptr(m_pResults[0]->Projection));
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
	if (m_pResults.empty())
	{
		return;
	}
	AvatarPoseGlm& avatar = m_pResults[0]->avatar;
	CameraPoseGlm& camera = m_pResults[0]->camera;
	glMatrixMode(GL_MODELVIEW);
	
	//gluLookAt(m_cameraPosition[Xaxis], m_cameraPosition[Yaxis], m_cameraPosition[Zaxis],
	//		  camera.position[Xaxis] + avatar.translation[Xaxis], camera.position[Yaxis] + avatar.translation[Yaxis], camera.position[Zaxis] + avatar.translation[Zaxis],
	//		  0, 1, 0);
	
	/*gluLookAt(camera.position[Xaxis], camera.position[Yaxis], camera.position[Zaxis],
			  camera.target[Xaxis],   camera.target[Yaxis],   camera.target[Zaxis],
			  camera.upVector[Xaxis], camera.upVector[Yaxis], camera.upVector[Zaxis]);*/
	/*
	glTranslatef(camera.position[Xaxis] + avatar.translation[Xaxis], 
				 camera.position[Yaxis] + avatar.translation[Yaxis], 
				 camera.position[Zaxis] + avatar.translation[Zaxis]);	
	float pitch = avatar.eulerAngles[Pitch];
	float yaw   = avatar.eulerAngles[Yaw];
	float roll  = avatar.eulerAngles[Roll];
	glRotatef(pitch, 1, 0, 0);
	glRotatef(yaw  , 0, 1, 0);
	glRotatef(roll , 0, 0, 1);	
	glScalef(avatar.scale[0][0], avatar.scale[1][1], avatar.scale[2][2]);*/
	
	glMultMatrixf(glm::value_ptr(m_pResults[0]->ModelView));

}

void  KinectView::RenderView()
{
	ResetView();	
	ResetScene();
	{
		glPushMatrix();
		SetupView();
		{
			glPushMatrix();
			SetupScene();
			RenderScene();
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