#include "MultiViewerWidget.h"

#ifdef _DEBUG
#include "vld.h"
#endif

float tvSizeWidth = 0.305 * 5; // 5 feet
float tvSizeHeight = 0.305 * 3; // 3 feet
float tvSizeDepth = 1.5 * 0.0254; // 1.5 inches
float tvPadHeight = 0.07; // 7 cm
float tvScreenWidth = 56 * 0.0254; // 56 inches
float tvScreenHeight = 32 * 0.0254; // 32 inches
float tvScreenDepth = 0 ; // 0

MultiViewerWidget::MultiViewerWidget(QWidget* parent, osg::ref_ptr<osg::DisplaySettings> ds, osg::ref_ptr<osg::GraphicsContext::Traits> traits) : QWidget(parent)
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
    m_qtWindow = new osgQt::GraphicsWindowQt(m_traits);
    QWidget* widget = m_qtWindow->getGLWidget();
    grid->addWidget(widget);
    setLayout(grid);
    setMouseTracking(true);

    RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );
}

MultiViewerWidget::~MultiViewerWidget() 
{
    if(m_qtWindow)
    {
        delete m_qtWindow;
        m_qtWindow = NULL;
    }
}

void MultiViewerWidget::CreateGraphicsWindow(float offsetX, float offsetZ)
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }
    
    unsigned int width=0, height=0;
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

        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());

        osg::ref_ptr<osg::Camera> camera = new osg::Camera();
        camera->setGraphicsContext(gc);
        camera->setViewport(new osg::Viewport(0,0, resolution.width, resolution.height));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);
        camera->setProjectionMatrixAsPerspective (45.0,resolution.width/resolution.height, 0.1, 10.0);
        osg::Vec3 offset = i == 0 ? osg::Vec3(-offsetX,0,-offsetZ) : osg::Vec3(offsetX,0,-offsetZ);
        camera->setViewMatrixAsLookAt(offset, osg::Vec3(0,0,0), osg::Vec3(0,0,1));
        camera->setReferenceFrame(osg::Camera::RELATIVE_RF);

        float bezelWidth = tvSizeWidth - tvScreenWidth;
        float resolutionX = traits->width / tvScreenWidth;
        float bezelOffsetInPixels = bezelWidth * resolutionX;
        float bezelOffsetNormalized = bezelOffsetInPixels / (bezelOffsetInPixels + resolutionX);
        float bezelOffsetProjection = bezelOffsetNormalized * cos(osg::DegreesToRadians(30.0));
        if (i) translate_x += bezelOffsetProjection + bezelOffsetNormalized; // 2 bezels
        addSlave(camera.get(), osg::Matrix::scale(1.0, 1.0, 1.0)*osg::Matrix::translate(translate_x, 0.0, 0.0), osg::Matrix::translate(0.0, 0.0, 0.0));
    }
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