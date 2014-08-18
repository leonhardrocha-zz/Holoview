#include "osgLib.h"
#ifndef _SCREENINFO_H
#define _SCREENINFO_H
#include "DisplayInfo.h"

class ScreenInfo : public DisplayInfo
{
public:
    ScreenInfo(double fovx = osg::inDegrees(60.0)) : DisplayInfo()
	{
		screenWidth = Width - 2 * BezelWidth;
		screenHeight = Height - 2 * BezelHeight;
		screenDepth = screenWidth > screenHeight ? screenWidth : screenHeight;
		left =  -screenWidth/2;
        right = screenWidth/2;
        bottom = -screenHeight/2;
        top = screenHeight/2;
        zNear = (right - left) / (2.0 * tan(fovx/2.0));
		zFar = zNear + screenDepth;
	};

	virtual osg::Matrix GetFrustum() { return osg::Matrix::frustum(left, right, bottom, top, zNear, zFar); } 
	virtual bool SetFrustum(osg::Matrix& frustum) { return frustum.getFrustum(left, right, bottom, top, zNear, zFar); }
	
	double screenWidth;
    double screenHeight;
    double screenDepth;

protected:

	double left, right, top, bottom, zNear, zFar;
};

#endif