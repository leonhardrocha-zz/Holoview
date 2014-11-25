#include "osgLib.h"
#ifndef _MULTIVIEWERWIDGET_H
#define _MULTIVIEWERWIDGET_H

class MultiViewerWidget : public QWidget, public osgViewer::Viewer
{
public:
    MultiViewerWidget(QWidget* parent = (QWidget*)NULL, osg::ref_ptr<osg::DisplaySettings> ds = NULL, osg::ref_ptr<osg::GraphicsContext::Traits> traits = NULL);
    ~MultiViewerWidget();
    virtual void CreateGraphicsWindow();
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    static osg::Matrix CalculateViewFrustum(const osg::Camera* slaveCamera, const osg::Vec3 eyePos, const double scaleFactor = 1.0);
    void SetStereoSettings();
    std::string leftTvName;
    std::string rightTvName;
    double m_ScreenRotationAngleInDegrees;
    std::vector<double> m_FocusPlaneDistance;
protected:
    virtual void paintEvent( QPaintEvent* event );
    QTimer _timer;
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    QWidget* m_parent;
    osgQt::GraphicsWindowQt* m_qtWindow;
    struct Frustum
    {
        double left, right, top, bottom, zNear, zFar;
    };

    Frustum m_Frustum[2];
};
#endif