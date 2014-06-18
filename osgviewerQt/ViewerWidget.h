#pragma once
#include <QTimer>
#include <QApplication>
#include <QGridLayout>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Camera>
#include <osgGA/CameraManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/OrbitManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <osg/PositionAttitudeTransform>
#include <iostream>
#include "OsgScene.h"
#include "OsgView.h"

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
    ViewerWidget(QWidget* parent = (QWidget*)NULL, osg::ref_ptr<osg::DisplaySettings> ds = NULL, osg::ref_ptr<osg::GraphicsContext::Traits> traits = NULL);
    ~ViewerWidget();
    virtual void CreateGraphicsWindow();
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    virtual void SetStereoSettings();

protected:
    virtual void paintEvent( QPaintEvent* event );
    QTimer _timer;
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    QWidget* m_parent;
};
