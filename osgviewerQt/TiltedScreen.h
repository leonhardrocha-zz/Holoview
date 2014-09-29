#include "osgLib.h"
#ifndef _TiltedScreenInfo_H
#define _TiltedScreenInfo_H
#include "ScreenInfo.h"
#include "MatrixExtension.h"

class  TiltedScreen : public ScreenInfo
{
public:
    TiltedScreen() : ScreenInfo() {};
    virtual osg::Matrix GetFrustum(const osg::Vec3& eye)=0;

};

class  RightScreen : public TiltedScreen
{
public:
    RightScreen() : TiltedScreen() {};
    virtual osg::Matrix GetFrustum(const osg::Vec3& eye) 
    {
        return MatrixExtension::getScreenProjection(osg::Vec3(BezelWidth * sin(FOV.x()), 0 , -screenHeight/2),
                                                    osg::Vec3((BezelWidth + screenWidth) * sin(FOV.x()), 0, -screenHeight/2),
                                                    osg::Vec3(BezelWidth * sin(FOV.x()), 0 , screenHeight/2),
                                                    eye,
                                                    zNear,
                                                    zFar);
    };
};

class  LeftScreen : public TiltedScreen
{
public:
    LeftScreen() : TiltedScreen() {};
    virtual osg::Matrix GetFrustum(const osg::Vec3& eye) 
    {
        return MatrixExtension::getScreenProjection(osg::Vec3(-(BezelWidth + screenWidth) * sin(FOV.x()), 0 , -screenHeight/2),
                                                    osg::Vec3(-BezelWidth * sin(FOV.x()), 0, -screenHeight/2),
                                                    osg::Vec3(-(BezelWidth + screenWidth) * sin(FOV.x()), 0 , screenHeight/2),
                                                    eye,
                                                    zNear,
                                                    zFar);
    };
};

#endif