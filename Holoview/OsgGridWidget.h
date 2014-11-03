#include "qtLib.h"
#ifndef _OSG_VIEWERWIDGET_H
#define _OSG_VIEWERWIDGET_H

#include "DockFrame.h"
#include "ViewerWidget.h"
#include "IArgs.h"

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <iostream>


class  OsgGridWidget :  public DockFrame
{
public:
    OsgGridWidget::OsgGridWidget(const QString &dockName, QWidget *parent) : DockFrame(dockName, parent)
    {
        m_viewer = new ViewerWidget(parent);
        m_grid = new QGridLayout;
        RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen);
        setLayout( m_grid );
    }

    void OsgGridWidget::Init(IArgs *viewArgs = NULL, std::string viewName = "")
    {
        m_viewer->setMouseTracking(true);
        if (viewArgs->Exists(viewName))
        {
            osgViewer::View* view = static_cast<osgViewer::View*>(viewArgs->Get(viewName));
            m_viewer->Init(view->getViewerBase(), view->getCamera());
        }
        else
        {
            m_viewer->Init();
        }
        m_viewer->CreateGraphicsWindow();
        QWidget* viewWidget = addViewWidget(m_viewer->GetMainCamera());
        m_grid->addWidget( viewWidget, 0, 0 );

        
        connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
        _timer.start( 10 );
    }

    QWidget* GetWidget() { return m_viewer; };
protected:

    QWidget* addViewWidget( osg::Camera* camera)
    {
        //osgViewer::View* view = new osgViewer::View;
        //view->setCamera( camera );
        //addView( view );

        //view->addEventHandler( new osgViewer::StatsHandler );
        //view->setCameraManipulator( new osgGA::TrackballManipulator );
        osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );
        return gw ? gw->getGraphWidget() : NULL;
    }

    osg::Camera* createCamera( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false )
    {
        osg::DisplaySettings* ds = osg::DisplaySettings::instance();
        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->windowName = name;
        traits->windowDecoration = windowDecoration;
        traits->x = x;
        traits->y = y;
        traits->width = w;
        traits->height = h;
        traits->doubleBuffer = true;
        traits->alpha = ds->getMinimumNumAlphaBits();
        traits->stencil = ds->getMinimumNumStencilBits();
        traits->sampleBuffers = ds->getMultiSamples();
        traits->samples = ds->getNumMultiSamples();
        
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setGraphicsContext( new osgQt::GraphicsWindowQt(traits.get()) );
        
        camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
        camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
        camera->setProjectionMatrixAsPerspective(
            30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );
        return camera.release();
    }

    QTimer _timer;
    ViewerWidget* m_viewer;
    QGridLayout* m_grid;
};

#endif