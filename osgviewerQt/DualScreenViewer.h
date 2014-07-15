#include "stdafx.h"
#ifndef _DualScreenViewer_H
#define _DualScreenViewer_H

class DualScreenViewer : public osgViewer::Viewer
{
public:
    DualScreenViewer();
    ~DualScreenViewer();
    virtual void CreateGraphicsWindow();
    virtual osg::Matrix CalculateView(const osg::View::Slave& slave, const osg::Vec3 eyePos);
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    osg::Matrix GetLeftProjectionOffsetMatrix() { return m_leftProjectionOffsetMatrix; };
    osg::Matrix GetRightProjectionOffsetMatrix() { return m_rightProjectionOffsetMatrix; };
    osg::View::Slave& GetLeftSlave() { return m_leftSlave; };
    osg::View::Slave& GetRightSlave() { return m_rightSlave; };

    void SetStereoSettings();
    std::string leftTvName;
    std::string rightTvName;
    std::vector<double> m_FocusPlaneDistance;



    osg::Vec3 m_EyeProjectionOffset;

protected:
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    struct Frustum
    {
        double left, right, top, bottom, zNear, zFar;
    };

    Frustum m_Frustum[2];

    osg::Matrix m_projectionMatrix;
    osg::Matrix m_rightProjectionOffsetMatrix;
    osg::Matrix m_leftProjectionOffsetMatrix;

    osg::View::Slave m_leftSlave;
    osg::View::Slave m_rightSlave;

    osg::Vec3 m_rightTVBasePosition;
    osg::Vec3 m_leftTVBasePosition;
    
    double m_tvElevation;
    double m_tvWidth;
    double m_tvHeight;
    double m_tvDepth;
    double m_bezelWidth;
    double m_bezelHeight;
    double m_screenWidth;
    double m_screenHeight;
    double m_screenDistance;
    double m_screenDepth;
    double m_angleBetweenScreensInDegrees;
};
#endif