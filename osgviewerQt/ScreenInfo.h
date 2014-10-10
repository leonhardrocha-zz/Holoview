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
        zNear = BezelWidth * cos(fovx);
        zFar = zNear + 2 * screenWidth;
        screenDepth = (zFar - zNear) / cos(fovx/2.0);
        m_frustum = osg::Matrix::perspective(60.0/ar, ar, zNear, zFar );
        //m_frustum.getFrustum(left, right, bottom, top, zNear, zFar);
        
    };

    virtual osg::Matrix GetFrustum() { return m_frustum; } 
    virtual bool SetFrustum(osg::Matrix& frustum) { m_frustum = frustum; return true; }
    osg::Vec2 FOV;
    double screenWidth;
    double screenHeight;
    double screenDepth;
protected:
    double /*left, right, top, bottom, */zNear, zFar;
    osg::Matrix m_frustum;
};

#endif