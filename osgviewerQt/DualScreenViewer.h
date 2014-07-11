#include "stdafx.h"
#ifndef _DualScreenViewer_H
#define _DualScreenViewer_H

class DualScreenViewer : public osgViewer::Viewer
{
public:
    DualScreenViewer();
    ~DualScreenViewer();
    virtual void CreateGraphicsWindow();
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    void SetStereoSettings();
    std::string leftTvName;
    std::string rightTvName;
    std::vector<double> m_FocusPlaneDistance;
protected:
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    struct Frustum
    {
        double left, right, top, bottom, zNear, zFar;
    };

    Frustum m_Frustum[2];

    double m_tvWidth;
    double m_tvHeight;
    double m_tvDepth;
    double m_bezelWidth;
    double m_bezelHeight;
    double m_screenWidth;
    double m_screenHeight;
    double m_screenDistance;
    double m_screenDepth;
    double m_angleBetweenScreenInDegrees;
};
#endif