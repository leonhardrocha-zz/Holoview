#include "stdafx.h"
#ifndef _DualScreenViewer_H
#define _DualScreenViewer_H

class DualScreenViewer : public osgViewer::Viewer
{
public:
    struct ScreenInfo
    {
        double left, right, top, bottom, zLeft, zRight;
        std::string name;
        osg::Vec3 center;
        osg::Plane viewPlane;
    };

    enum TV_SIDE
    {
        Left = 0,
        Right = 1,
        NumOfScreens = 2
    };

    DualScreenViewer();
    ~DualScreenViewer();
    virtual void CreateGraphicsWindow();
    virtual void CalculateProjectionMatrixOffset(osg::Vec3 eye);
    virtual void CalculateProjectionMatrixOffset();
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    osg::Vec3& GetCenter() { return m_virtualCenter; };
    void SetStereoSettings();
    std::vector<double> m_FocusPlaneDistance;



    osg::Vec3 m_EyeProjectionOffset;

protected:
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;

    osg::Matrix m_viewMatrix;
    osg::Matrix m_projectionMatrix;
    osg::Matrix m_viewMatrixOffset[2];
    osg::Matrix m_projectionMatrixOffset[2];
    ScreenInfo m_screen[2];

    osg::Vec3 m_virtualCenter;
    osg::Vec3 m_origin;
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