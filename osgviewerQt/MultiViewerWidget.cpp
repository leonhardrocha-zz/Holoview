#include "MultiViewerWidget.h"
#include <math.h>
#include <iostream>
#ifdef _DEBUG
#include "vld.h"
#endif

double tvWidth = 60 * 0.0254; // 60 inches
double tvHeight = 36 * 0.0254; // 36 inches 
double tvDepth = 1.5 * 0.0254; // 1.5 inches
double bezelWidth = 2 * 0.0254; // 2 inches
double bezelHeight = 2 * 0.0254; //2 inches
double screenWidth = 56 * 0.0254; // 56 inches
double screenHeight = 32 * 0.0254; // 32 inches
double screenDepth = 1.0 ; // 0


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
    leftTvName = "LeftTV";
    rightTvName = "RightTV";
}

MultiViewerWidget::~MultiViewerWidget() 
{
    if(m_qtWindow)
    {
        delete m_qtWindow;
        m_qtWindow = NULL;
    }
}

void MultiViewerWidget::CreateGraphicsWindow()
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

    double bezelWidth = tvWidth - screenWidth;
    double angleInRadians = osg::inDegrees(120.0);
    double angleInDegrees = osg::RadiansToDegrees(angleInRadians);
    double offsetNormalized = tvWidth / screenWidth;
    double depthOffSetNormalized = offsetNormalized * cos(angleInRadians);
    double bezelOffsetProjection = offsetNormalized * sin(angleInRadians);

    osg::Camera* viewCamera = getCamera();

    for(unsigned int i=0; i<numCameras;++i)
    {
        osg::GraphicsContext::ScreenIdentifier screenId = osg::GraphicsContext::ScreenIdentifier(i);
        screenId.setUndefinedScreenDetailsToDefaultScreen();
        screenId.readDISPLAY();
        osg::GraphicsContext::ScreenSettings resolution;
        wsi->getScreenSettings(screenId, resolution);
        double aspectRatio = (double)resolution.width/(double)resolution.height;
        viewCamera->setProjectionMatrixAsPerspective(angleInDegrees/2 * aspectRatio, aspectRatio, 0.1, 3.0);
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

        double left, right, bottom, top, zNear, zFar;
        osg::Matrixd viewMatrix = osg::Matrixd::translate(0.0,0.0,0.0);

        /*left = -0.5;
        right = 0.5;
        top = left * aspectRatio;
        bottom = right * aspectRatio;
        zNear= 0.1;
        zFar = 100.0;
        viewCamera->setProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);*/
        if (viewCamera->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar))
        {
            camera->setProjectionMatrixAsFrustum(i ? left : 0, i ? 0 : right, bottom, top, zNear, zFar);
        }
        viewCamera->setProjectionMatrix(osg::Matrixd());
        osg::Matrixd projectionMatrix = camera->getProjectionMatrix();
        camera->setName(i ? leftTvName : rightTvName);
        addSlave(camera.get(), projectionMatrix, viewMatrix);
        
    }
}

void MultiViewerWidget::SetStereoSettings()
{
    m_displaySettings->setStereo(true);
    m_displaySettings->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    m_displaySettings->setDisplayType(osg::DisplaySettings::MONITOR);
    m_displaySettings->setScreenDistance(screenDepth); 
    m_displaySettings->setScreenHeight(screenHeight);
    m_displaySettings->setScreenWidth(screenWidth);
    m_displaySettings->setEyeSeparation(0.06f); 
}

void MultiViewerWidget::paintEvent( QPaintEvent* event )
{ 
	frame(); 
}