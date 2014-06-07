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

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
    ViewerWidget(QWidget* parent = (QWidget*)NULL);
    ~ViewerWidget();
    QWidget* CreateGraphicsWindow(osg::DisplaySettings* ds, osg::ref_ptr<osg::GraphicsContext::Traits> traits);
    void Init();

    osg::ref_ptr<osgViewer::View> GetView() { return m_osgView; };
    void SetView(osg::ref_ptr<osgViewer::View> view) { m_osgView = view; };

protected:
    virtual void paintEvent( QPaintEvent* event );
    QTimer _timer;
    osg::ref_ptr<osgViewer::View> m_osgView;
    osgQt::GraphicsWindowQt* m_qtWindow;
    QWidget* m_qtWidget;
    QWidget* m_parent;
};
