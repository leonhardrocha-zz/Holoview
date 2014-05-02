// ToeInStereoView.cpp : implementation of the ToeInStereoView class
//
#include "ToeInStereoView.h"
#include <cmath>
#define PI 3.14159265
ToeInStereoView::ToeInStereoView()
{
}

void ToeInStereoView::SetupView()
{
	fovy = 45;                                          //field of view in y-axis
	aspect = double(windowWidth)/double(windowHeight);		//screen aspect ratio
	nearZ = 0.1;                                        //near clipping plane
	farZ = 10.0;                                        //far clipping plane
	IOD = 0.1;
	
	CameraPosition[0] = 0;
	CameraPosition[1] = 0;
	CameraPosition[2] = 3;
	LookAtPosition[0] = 0;
	LookAtPosition[1] = 0;
	LookAtPosition[2] = -1;
	LightPosition[0] = 0;
	LightPosition[1] = 0;
	LightPosition[2] = 0;

	SetupDualStereoView();

	CameraRotationAngle = 30.0;
	CameraRotationShift[0] = (float)(LookAtPosition[0] * sin(CameraRotationAngle*PI/180));
	CameraRotationShift[1] = (float)(LookAtPosition[1] * sin(CameraRotationAngle*PI/180));
	CameraRotationShift[2] = (float)(LookAtPosition[2] * sin(CameraRotationAngle*PI/180));

}

void ToeInStereoView::RenderLeftView()
{
	leftStereoView.ResetLeftView();
			
	glRotatef(-CameraRotationAngle, 0.0, 1.0, 0.0);

	leftStereoView.RenderLeftView();

	leftStereoView.ResetRightView();
			
	glRotatef(-CameraRotationAngle, 0.0, 1.0, 0.0);

	leftStereoView.RenderRightView();
}

void ToeInStereoView::RenderRightView()
{	
	rightStereoView.ResetLeftView();

	glRotatef(CameraRotationAngle, 0.0, 1.0, 0.0);

	rightStereoView.RenderLeftView();
	
	rightStereoView.ResetRightView();

	glRotatef(CameraRotationAngle, 0.0, 1.0, 0.0);

	rightStereoView.RenderRightView();
}

