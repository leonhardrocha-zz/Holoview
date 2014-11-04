#include "ViewerWidget.h"

#ifdef _DEBUG
#include "vld.h"
#endif

void ViewerWidget::Init(osgViewer::ViewerBase* viewer, osg::Camera* camera)
{
    m_viewer = osg::ref_ptr<osgViewer::ViewerBase>(viewer);
#ifndef WIN32
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
#endif
    m_viewer->setKeyEventSetsDone(0);

    m_displaySettings = osg::DisplaySettings::instance().get();

    m_traits = CreateDefaultTraits(m_displaySettings, m_viewer->getName() + " View");

    m_camera = osg::ref_ptr<osg::Camera>((camera == NULL) ? CreateDefaultCamera(m_traits) : camera);

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
    grid->addWidget(widget, 0, 0 );
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
    m_viewer->frame(); 
}

bool ViewerWidget::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::Paint) 
    {
        paintEvent((QPaintEvent *)e);
    }
    return QWidget::eventFilter(o, e);
}

osg::Camera* ViewerWidget::CreateDefaultCamera(osg::GraphicsContext::Traits* traits)
{
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setGraphicsContext( new osgQt::GraphicsWindowQt(traits));
    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );
    return camera.release();
}

osg::GraphicsContext::Traits* ViewerWidget::CreateDefaultTraits(osg::DisplaySettings* ds, const std::string& windowName)
{
    osg::GraphicsContext::Traits* traits =  new osg::GraphicsContext::Traits(ds);
    
    traits->windowName = windowName;
    traits->windowDecoration = isWindow();
    traits->x = 0;
    traits->y = 0;
    traits->width = width();
    traits->height = height();
    traits->doubleBuffer = true;
    traits->alpha = m_displaySettings->getMinimumNumAlphaBits();
    traits->stencil = m_displaySettings->getMinimumNumStencilBits();
    traits->sampleBuffers = m_displaySettings->getMultiSamples();
    traits->samples = m_displaySettings->getNumMultiSamples();

    return traits;
}