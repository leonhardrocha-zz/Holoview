#include "ViewerWidget.h"

#ifdef _DEBUG
#include "vld.h"
#endif

void ViewerWidget::Init(osgViewer::ViewerBase* viewer, osg::Camera* camera)
{
    m_viewer = viewer;
    m_camera = camera;

    m_displaySettings = osg::DisplaySettings::instance().get();
    m_traits = new osg::GraphicsContext::Traits;
    
#ifndef WIN32
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
#endif
    setKeyEventSetsDone(0);
    m_traits->windowName = getName();
    m_traits->windowDecoration = isWindow();
    m_traits->x = 0;
    m_traits->y = 0;
    m_traits->width = width();
    m_traits->height = height();
    m_traits->doubleBuffer = true;
    m_traits->alpha = m_displaySettings->getMinimumNumAlphaBits();
    m_traits->stencil = m_displaySettings->getMinimumNumStencilBits();
    m_traits->sampleBuffers = m_displaySettings->getMultiSamples();
    m_traits->samples = m_displaySettings->getNumMultiSamples();

    QGridLayout* grid = new QGridLayout;
    setLayout(grid);
    setMouseTracking(true);

    RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );
}

void ViewerWidget::CreateGraphicsWindow()
{
    osgQt::GraphicsWindowQt* qtWindow = new osgQt::GraphicsWindowQt(m_traits.get());
    m_camera->setGraphicsContext( qtWindow );
    m_camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    m_camera->setViewport( new osg::Viewport(0, 0, m_traits->width, m_traits->height) );
    m_camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(m_traits->width)/static_cast<double>(m_traits->height), 1.0f, 10000.0f );
    QWidget* widget = qtWindow->getGLWidget();
    QGridLayout* grid = static_cast<QGridLayout*>(layout());
    grid->addWidget(widget, 0, getNumViews() );
    osgViewer::ViewerBase::Views views;
    m_viewer->getViews(views);
    if (!views.empty())
    {
        for (auto view = views.begin(); view != views.end(); view++)
        { 
            addView(*view);
        }
    }
}

void ViewerWidget::SetStereoSettings()
{
    m_displaySettings->setStereo(true);
    m_displaySettings->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    m_displaySettings->setDisplayType(osg::DisplaySettings::MONITOR);
    m_displaySettings->setSplitStereoHorizontalEyeMapping(osg::DisplaySettings::LEFT_EYE_LEFT_VIEWPORT);
}

void ViewerWidget::UnsetStereoSettings()
{
    m_displaySettings->setStereo(false);
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{ 
    frame(); 
}
