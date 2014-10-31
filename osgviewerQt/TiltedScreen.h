#include "osgLib.h"
#ifndef _TiltedScreenInfo_H
#define _TiltedScreenInfo_H
#include "ScreenInfo.h"
#include "MatrixExtension.h"

class  TiltedScreen : public ScreenInfo
{
public:
    TiltedScreen() : ScreenInfo() {};
    TiltedScreen(const TiltedScreen& scr) : ScreenInfo(scr), m_bottomLeft(scr.m_bottomLeft), m_topLeft(scr.m_topLeft), m_bottomRight(scr.m_bottomRight)  {};
    TiltedScreen(const osg::Vec3& bottomLeft, const osg::Vec3& topLeft, const osg::Vec3& bottomRight) : ScreenInfo(), m_bottomLeft(bottomLeft), m_topLeft(topLeft), m_bottomRight(bottomRight), m_topRight(osg::Vec3(m_bottomRight.x(), m_topLeft.y(), m_bottomRight.z())) {};
    virtual void Update(const osg::Vec3& eye)
    {
        osg::Vec3 va, vb, vc;
        osg::Vec3 vr, vu, vn;

        // Compute an orthonormal basis for the screen.
        vr = m_bottomRight - m_bottomLeft;
        vu = m_topLeft- m_bottomLeft;
        vr.normalize();
        vu.normalize();
        vn  = vr ^ vu;
        vn.normalize();
    
        // Compute the screen corner vectors.
        va = m_bottomLeft - eye;
        vb = m_bottomRight - eye;
        vc = m_topLeft - eye;

        // Find the distance from the eye to screen plane.
        screenDistance = -(va * vn);
        // Find the extent of the perpendicular projection.
        left = (vr * va) * zNear / screenDistance;
        right = (vr * vb) * zNear / screenDistance;
        bottom = (vu * va) * zNear / screenDistance;
        top = (vu * vc) * zNear / screenDistance;

         // Load the perpendicular projection.

        // Rotate the projection to be non-perpendicular.
        osg::Matrix Mrot(vr.x(), vu.x(), vn.x(), 0,
                         vr.y(), vu.y(), vn.y(), 0,
                         vr.z(), vu.z(), vn.z(), 0,
                         0, 0, 0, 1);
        m_view = Mrot;
        zFar = screenDistance;
        m_frustum = osg::Matrix::frustum(left, right, bottom, top, zNear, zFar);
    }
    
    osg::Vec3 m_topLeft, m_bottomLeft, m_bottomRight, m_topRight;
    double screenDistance;
};

//class  RightScreen : public TiltedScreen
//{
//public:
//    RightScreen() : TiltedScreen(osg::Vec3(m_info.BezelWidth * sin(m_info.FOV.x()), 0 , -m_info.screenHeight/2),
//                                 osg::Vec3((m_info.BezelWidth + m_info.screenWidth) * sin(m_info.FOV.x()), 0, -m_info.screenHeight/2),
//                                 osg::Vec3(m_info.BezelWidth * sin(m_info.FOV.x()), 0 , m_info.screenHeight/2)) {};
//    protected:
//        ScreenInfo m_info;
//};
//
//class  LeftScreen : public TiltedScreen
//{
//public:
//    LeftScreen() : TiltedScreen(osg::Vec3(-(m_info.BezelWidth + screenWidth) * sin(FOV.x()), 0 , -m_info.screenHeight/2),
//                                osg::Vec3(-m_info.BezelWidth * sin(FOV.x()), 0, -m_info.screenHeight/2),
//                                osg::Vec3(-(m_info.BezelWidth + screenWidth) * sin(FOV.x()), 0 , m_info.screenHeight/2)) {};
//    protected:
//        ScreenInfo m_info;
//
//};

#endif