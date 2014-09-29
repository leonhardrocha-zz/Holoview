#include "osgLib.h"
#ifndef _SCREENINFO_H
#define _SCREENINFO_H
#include "DisplayInfo.h"
#include "MatrixExtension.h"

class DualScreenViewer;
class TiltedScreen;

class ScreenInfo : public DisplayInfo
{
    friend class DualScreenViewer;
    friend class TiltedScreen;
public:
    ScreenInfo(double fovx = osg::inDegrees(60.0)) : DisplayInfo()
    {
        screenWidth = Width - 2 * BezelWidth;
        screenHeight = Height - 2 * BezelHeight;
        double ar = screenWidth/screenHeight;
        FOV = osg::Vec2(fovx, fovx/ar);
        left = -screenWidth/2;
        right = screenWidth/2;
        bottom = -screenHeight/2;
        top = screenHeight/2;
        zNear = right/tan(fovx/2.0);
        double tiltAngle = osg::inDegrees(60.0);
        zFar = zNear + screenWidth * cos(tiltAngle);
        screenDepth = zFar - zNear;
    };

    virtual osg::Matrix GetFrustum() { return osg::Matrix::frustum(left, right, bottom, top, zNear, zFar); } 
    virtual bool SetFrustum(osg::Matrix& frustum) { return frustum.getFrustum(left, right, bottom, top, zNear, zFar); }
    osg::Vec2 FOV;
    double screenWidth;
    double screenHeight;
    double screenDepth;
protected:
    double left, right, top, bottom, zNear, zFar;
};

class HoloScreenInfo : public ScreenInfo
{
    friend class DualScreenViewer;
    virtual osg::Matrix GetFrustum() { return MatrixExtension::HolographicFrustum(left, right, bottom, top, zNear, zFar); } 
};

#endif