#include "ViewerWidget.h"

#ifdef _DEBUG
#include "vld.h"
#endif

ViewerWidget::ViewerWidget(QWidget* parent, osg::ref_ptr<osg::DisplaySettings> ds, osg::ref_ptr<osg::GraphicsContext::Traits> traits) : QWidget(parent)
{
    if (ds.valid())
    {
        m_displaySettings = ds;
    }
    else
    {
        m_displaySettings = osg::DisplaySettings::instance().get();
    }
    
    if (traits.valid())
    {
        m_traits = traits;
    }
    else
    {
        m_traits = new osg::GraphicsContext::Traits;
    }

    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
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

ViewerWidget::~ViewerWidget() 
{
}

void ViewerWidget::CreateGraphicsWindow()
{
    osgViewer::View* view = new osgViewer::View;
    osg::Camera* camera = view->getCamera();

    osgQt::GraphicsWindowQt* qtWindow = new osgQt::GraphicsWindowQt(m_traits.get());
    camera->setGraphicsContext( qtWindow );
    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, m_traits->width, m_traits->height) );
    camera->setViewMatrixAsLookAt(osg::Vec3(0,0,0), osg::Vec3(0,0,-1), osg::Vec3(0,1,0));
    /*camera->setProjectionMatrixAsOrtho(-1, 1, -1, 1, -1, 1);*/
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(m_traits->width)/static_cast<double>(m_traits->height), 1.0f, 10000.0f );
    QWidget* widget = qtWindow->getGLWidget();
    QGridLayout* grid = static_cast<QGridLayout*>(layout());
    grid->addWidget(widget, 0, getNumViews() );
    addView(view);
}

void ViewerWidget::SetStereoSettings()
{
    m_displaySettings->setStereo(true);
    m_displaySettings->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    m_displaySettings->setDisplayType(osg::DisplaySettings::MONITOR);
    m_displaySettings->setSplitStereoHorizontalEyeMapping(osg::DisplaySettings::LEFT_EYE_LEFT_VIEWPORT);
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{ 
	frame(); 
}