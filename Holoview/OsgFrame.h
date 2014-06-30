#ifndef _OSG_DOCK_H
#define _OSG_DOCK_H

#include <QWidget>
#include <QEvent>
#include <QPaintEngine>
#include <QThread>
#include "DockFrame.h"
#include "ViewerWidget.h"

class  OSGFrame :  public DockFrame
{
public:
	OSGFrame::OSGFrame(const QString &dockName, QWidget *parent, osg::ref_ptr<osgViewer::Scene> scene = NULL) : DockFrame(dockName, parent)
	{
        RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
		setAttribute(Qt::WA_NativeWindow);
		setAttribute(Qt::WA_PaintOnScreen);

        worldViewer.CreateGraphicsWindow();
        worldViewer.setMouseTracking(true);
        osg::ref_ptr<osgViewer::View> view = worldViewer.getView(0);
        if (scene.valid())
        {
            m_scene = scene;
            view->setSceneData( scene->getSceneData() );
        }
        view->addEventHandler( new osgViewer::StatsHandler );
        view->setCameraManipulator( new osgGA::TrackballManipulator );
    }

protected:

    osg::ref_ptr<osgViewer::Scene> m_scene;
    ViewerWidget worldViewer;

};

#endif