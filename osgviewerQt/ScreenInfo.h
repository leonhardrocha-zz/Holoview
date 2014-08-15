#include "osgLib.h"
#ifndef _SCREENINFO_H
#define _SCREENINFO_H
#include "BaseScreenInfo.h"

class ScreenInfo : public BaseScreenInfo
{
public:
    ScreenInfo() : BaseScreenInfo() { Update(); };
    ScreenInfo(double angle) : BaseScreenInfo(), rotationAngle(angle) { Update(); };
public:
    std::string name;
    osg::Vec3 center;
    osg::Vec3 m_basePosition;
    osg::Plane viewPlane;
    osg::Matrix rotation;
    double rotationAngle;

    virtual void Update() 
    {
        double cosine = std::cos(rotationAngle);
        double sine = std::sin(rotationAngle);
        left =  m_bezelWidth  * sine;
        right = (m_bezelWidth + m_screenWidth) * sine;
        bottom = m_tvElevation + m_bezelWidth;
        top = m_tvElevation + m_bezelWidth + m_screenHeight;
        zFar = m_bezelWidth * cosine;
        zNear = (m_bezelWidth + m_screenWidth) * cosine;
        zRight = zNear;
        zLeft = zFar;
        center = osg::Vec3((left + right)/2.0, (top + bottom)/2.0, (zLeft + zRight)/2.0);
        m_basePosition = osg::Vec3(m_tvWidth / 2.0 * sine, m_tvElevation, m_tvWidth /2.0 * cosine);
    };
};

#endif