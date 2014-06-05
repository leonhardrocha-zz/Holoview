#pragma once
#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <iostream>
#include "OsgScene.h"
#include "OsgView.h"

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
    ViewerWidget(QWidget* parent = (QWidget*)0, osg::ref_ptr<osgViewer::View> view = NULL, osg::ref_ptr<osg::Group> scene = NULL);
    ~ViewerWidget();
    QWidget* addViewWidget(osgViewer::View *view, osgQt::GraphicsWindowQt* gw, osg::Node* scene );
    static  osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false);
    
    void Init();

    osg::ref_ptr<osgViewer::View> GetView() { return m_osgView; };
    void SetView(osg::ref_ptr<osgViewer::View> view) { m_osgView = view; };
    
    osg::ref_ptr<osg::Group> GetScene() { return m_osgScene; };
    void SetScene(osg::ref_ptr<osg::Group> scene) { m_osgScene = scene; };


protected:
    virtual void paintEvent( QPaintEvent* event );
    QTimer _timer;
    osg::ref_ptr<osgViewer::View> m_osgView;
    osg::ref_ptr<osg::Group> m_osgScene;
    QWidget* m_qtWidget;
};
