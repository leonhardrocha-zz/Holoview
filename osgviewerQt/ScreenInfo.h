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
    //friend class TiltedScreen;
public:
    ScreenInfo(double fovx = osg::inDegrees(60.0), double range = DisplayInfo::inMeters(100.0)) : DisplayInfo(), ViewRange(range)
    {
        screenWidth = Width - 2 * BezelWidth;
        screenHeight = Height - 2 * BezelHeight;
        double ar = screenWidth/screenHeight;
        FOV = osg::Vec2(fovx, fovx/ar);
        zNear = BezelWidth * cos(fovx);
        zFar = screenWidth * cos(fovx) + ViewRange; // virtual screen + view ange
        screenDepth = screenWidth * cos(fovx/2.0);
        m_frustum = osg::Matrix::perspective(60.0/ar, ar, zNear, zFar );
        m_frustum.getFrustum(left, right, bottom, top, zNear, zFar);
        
    };

    virtual osg::Matrix GetView() { return m_view; } 
    virtual osg::Matrix GetFrustum() { return m_frustum; } 
    virtual bool SetFrustum(osg::Matrix& frustum) { m_frustum = frustum; return true; }
    osg::Vec2 FOV;
    double screenWidth;
    double screenHeight;
    double screenDepth;
    double ViewRange;
protected:
    double left, right, top, bottom, zNear, zFar;
    osg::Matrix m_frustum;
    osg::Matrix m_view;
};

#endif