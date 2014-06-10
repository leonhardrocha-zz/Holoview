#include "MultiViewerWidget.h"

#ifdef _DEBUG
#include "vld.h"
#endif

MultiViewerWidget::MultiViewerWidget(QWidget* parent) : QWidget(parent)
{
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    setKeyEventSetsDone(0);
    m_displaySettings = osg::DisplaySettings::instance().get();

    m_traits = new osg::GraphicsContext::Traits;
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

    RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );
}

MultiViewerWidget::~MultiViewerWidget() 
{
}

void MultiViewerWidget::Init()
{
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

    RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );
}




QWidget* MultiViewerWidget::CreateGraphicsWindow(osg::ref_ptr<osg::DisplaySettings> ds, osg::ref_ptr<osg::GraphicsContext::Traits> traits)
{
    if (ds.valid())
    {
        m_displaySettings = ds;
    }
    
    if (traits.valid())
    {
        m_traits = traits;
    }
    
   
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return NULL;
    }
    
    unsigned int width=0, height=0;

    osgQt::GraphicsWindowQt* qtWindow = new osgQt::GraphicsWindowQt(m_traits);
    unsigned int numCameras = 2;
    double aspectRatioScale = (double)numCameras;
    double translate_x = -1.0;
    for(unsigned int i=0; i<numCameras;++i, translate_x += 2.0)
    {
        osg::GraphicsContext::ScreenIdentifier screenId = osg::GraphicsContext::ScreenIdentifier(i);
        screenId.setUndefinedScreenDetailsToDefaultScreen();
        screenId.readDISPLAY();
        osg::GraphicsContext::ScreenSettings resolution;
        wsi->getScreenSettings(screenId, resolution);
        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->screenNum = screenId.screenNum;
        traits->x = 0;
        traits->y = 0;
        traits->width = resolution.width / (numCameras - 1);
        traits->height = resolution.height;
        traits->windowDecoration = false;
        traits->doubleBuffer = true;
        traits->sharedContext = 0;
        width += resolution.width;
        height = resolution.height;
        osg::ref_ptr<osg::GraphicsContext> gc = qtWindow->createGraphicsContext(traits);

        /*osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());*/

        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setGraphicsContext(gc);
        camera->setViewport(new osg::Viewport(0,0, resolution.width, resolution.height));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);

        addSlave(camera.get(), osg::Matrix::scale(1.0, 1.0, 1.0)*osg::Matrix::translate(translate_x, 0.0, 0.0), osg::Matrix() );
    }

    m_traits->x = 0;
    m_traits->y = 0;
    m_traits->width = width;
    m_traits->height = height;
    m_traits->windowDecoration = false;
    m_traits->doubleBuffer = true;
    m_traits->sharedContext = 0;


    QWidget* widget = qtWindow->getGLWidget();
    layout()->addWidget(widget);
}

void MultiViewerWidget::SetStereoSettings()
{
    m_displaySettings->setStereo(true);
    m_displaySettings->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    m_displaySettings->setDisplayType(osg::DisplaySettings::MONITOR);
    /*m_displaySettings->setSplitStereoHorizontalEyeMapping(osg::DisplaySettings::LEFT_EYE_LEFT_VIEWPORT);*/
}

void MultiViewerWidget::paintEvent( QPaintEvent* event )
{ 
	frame(); 
}