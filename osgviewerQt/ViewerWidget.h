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

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
    ViewerWidget(QWidget* parent = (QWidget*)0);
    QWidget* addViewWidget( osgQt::GraphicsWindowQt* gw, osg::Node* scene );
    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );
    void Init();
    OsgScene GetScene() { return m_scene; };
    void SetScene(OsgScene scene) { m_scene = scene; };

protected:
	virtual void paintEvent( QPaintEvent* event );	
    QTimer _timer;
    OsgScene m_scene;
};
