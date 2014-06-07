#include "ViewerWidget.h"

#ifdef _DEBUG
#include "vld.h"
#endif

ViewerWidget::ViewerWidget(QWidget* parent) : QWidget(parent)
{
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    setKeyEventSetsDone(0);
}

ViewerWidget::~ViewerWidget() 
{
}

void ViewerWidget::Init()
{
    QGridLayout* grid = new QGridLayout;
    grid->addWidget( m_qtWidget, 0, 0 );
    setLayout( grid );
    RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );
}

QWidget* ViewerWidget::CreateGraphicsWindow(osg::DisplaySettings* ds, osg::ref_ptr<osg::GraphicsContext::Traits> traits)
{
    m_qtWindow = new osgQt::GraphicsWindowQt(traits.get());
    addView(m_osgView);

    osg::Camera* camera = m_osgView->getCamera();
    camera->setGraphicsContext( m_qtWindow );
    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    m_qtWidget = m_qtWindow->getGLWidget();
    return m_qtWidget;
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{ 
	frame(); 
}