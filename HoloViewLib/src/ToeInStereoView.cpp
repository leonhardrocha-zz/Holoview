// ToeInStereoView.cpp : implementation of the ToeInStereoView class
//
#include "ToeInStereoView.h"
#include <cmath>
#define PI 3.14159265
ToeInStereoView::ToeInStereoView() : DualStereoView()
{
	Init();
}

void ToeInStereoView::Init()
{
	fovy = 45;                                          //field of view in y-axis
	nearZ = 0.1;                                        //near clipping plane
	farZ = 10.0;                                        //far clipping plane
	IOD = 0.1;
	
	CameraPosition[0] = 0;
	CameraPosition[1] = 0;
	CameraPosition[2] = 1;
	LookAtPosition[0] = 0;
	LookAtPosition[1] = 0;
	LookAtPosition[2] =-1;
	LightPosition[0]  = 0;
	LightPosition[1]  = 0;
	LightPosition[2]  = 1;
	
	leftViewAngles[3]  = 1.0;
	rightViewAngles[3] = 1.0;
	
}

void ToeInStereoView::SetupView()
{

	leftStereoView.SetupView();
	rightStereoView.SetupView();
}

void ToeInStereoView::SetViewAngles(float pitch, float yaw, float roll)
{
	leftViewVectors.setColumn(0, CameraPosition);
	leftViewVectors.setColumn(1, LookAtPosition);
	leftViewVectors.setColumn(2, LightPosition);
	
	leftViewAngles[ViewAngles::Pitch] = pitch;
	leftViewAngles[ViewAngles::Yaw]   = yaw;
	leftViewAngles[ViewAngles::Roll]  = roll;

	leftViewVectors.rotateX(pitch);
	leftViewVectors.rotateY(yaw);
	leftViewVectors.rotateZ(roll);

	rightViewVectors.setColumn(0, CameraPosition);
	rightViewVectors.setColumn(1, LookAtPosition);
	rightViewVectors.setColumn(2, LightPosition);

	rightViewAngles[ViewAngles::Pitch] = pitch;
	rightViewAngles[ViewAngles::Yaw]   = -yaw;
	rightViewAngles[ViewAngles::Roll]  = roll;

	rightViewVectors.rotateX(pitch);
	rightViewVectors.rotateY(-yaw);
	rightViewVectors.rotateZ(roll);
}

void ToeInStereoView::RenderLeftView()
{
	leftStereoView.ResetLeftView();

	const float* m = leftViewVectors.get();
			
	gluLookAt(m[0],
			  m[4],
			  m[8],
			  m[1],
			  m[5],
			  m[9],
			  0.0,
			  1.0,
			  0.0);

	leftStereoView.RenderLeftView();

	leftStereoView.ResetRightView();
	
	
	gluLookAt(m[0],
			  m[4],
			  m[8],
			  m[1],
			  m[5],
			  m[9],
			  0.0,
			  1.0,
			  0.0);

	leftStereoView.RenderRightView();
}

void ToeInStereoView::RenderRightView()
{	
	const float* m = leftViewVectors.get();
			
	rightStereoView.ResetLeftView();

	gluLookAt(m[0],
			  m[4],
			  m[8],
			  m[1],
			  m[5],
			  m[9],
			  0.0,
			  1.0,
			  0.0);

	rightStereoView.RenderLeftView();
	
	rightStereoView.ResetRightView();

	gluLookAt(m[0],
			  m[4],
			  m[8],
			  m[1],
			  m[5],
			  m[9],
			  0.0,
			  1.0,
			  0.0);

	rightStereoView.RenderRightView();
}

