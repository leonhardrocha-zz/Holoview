#pragma once
#include <QTimer>
#include <QApplication>
#include <QGridLayout>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Camera>
#include <osgGA/CameraManipulator>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>

#include <iostream>
#include "OsgScene.h"
#include "OsgView.h"

class MultiViewerWidget : public QWidget, public osgViewer::Viewer
{
public:
    MultiViewerWidget(QWidget* parent = (QWidget*)NULL, osg::ref_ptr<osg::DisplaySettings> ds = NULL, osg::ref_ptr<osg::GraphicsContext::Traits> traits = NULL);
    ~MultiViewerWidget();
    virtual void CreateGraphicsWindow(float offsetX, float offsetZ);
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    void SetStereoSettings();

protected:
    virtual void paintEvent( QPaintEvent* event );
    QTimer _timer;
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    QWidget* m_parent;
    osgQt::GraphicsWindowQt* m_qtWindow;
};
