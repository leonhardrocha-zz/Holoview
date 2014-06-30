#include "stdafx.h"
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
    void SetStereoSettings();
    std::string leftTvName;
    std::string rightTvName;

protected:
    virtual void paintEvent( QPaintEvent* event );
    QTimer _timer;
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    QWidget* m_parent;
    osgQt::GraphicsWindowQt* m_qtWindow;
};
#endif