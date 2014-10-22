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
double screenDistance = 50 * 0.0254; // 50 inches
double screenDepth = 1.0 ; // 0
double screenRotationAngleInDegrees = 120.0;

//#define SHEAR_LEFT
//#define SHEAR_RIGHT

osg::Matrix MultiViewerWidget::CalculateViewFrustum(const osg::Camera* slaveCamera, const osg::Vec3 eyePos, const double scaleFactor)
{
    double angleInRadians = osg::inDegrees(screenRotationAngleInDegrees);
    double acuteAngleInRadians = osg::PI_2 - angleInRadians/2.0;


    // Compute transformed eye position
    // - Converts eye coords into the surface's coord system
    // Xformed position of eyes
       

    double originToScreen, originToRight, originToLeft, originToTop, originToBottom;
    originToBottom = 0.74;
    originToTop = originToBottom + screenHeight;
    originToScreen = 0.0;//tvWidth /2.0 * sin (acuteAngleInRadians);
    osg::Matrix inverseView =  slaveCamera->getInverseViewMatrix();
    if (slaveCamera->getName() == "RightTV")
    {
        originToLeft = bezelWidth  * cos(acuteAngleInRadians);
        originToRight = (bezelWidth + screenWidth) * cos(acuteAngleInRadians);

    }
    else
    {
        originToLeft =  -(bezelWidth + screenWidth) * cos(acuteAngleInRadians);
        originToRight = -bezelWidth * cos(acuteAngleInRadians);
    }
    const osg::Vec3 eye_surface =  inverseView * eyePos;
    // Distance measurements from eye to screen/edges
    // Distance to edges is from the point on the screen plane
    // where a normal line would go through the origin.
    double eye_to_screen, eye_to_right, eye_to_left, eye_to_top, eye_to_bottom;
    // Compute dist from eye to screen/edges
    // Take into account scale since all origin to anythings are in meters
    eye_to_screen = (scaleFactor * originToScreen) + eye_surface.z();
    eye_to_right  = (scaleFactor * originToRight) - eye_surface.x();
    eye_to_left   = (scaleFactor * originToLeft) + eye_surface.x();
    eye_to_top    = (scaleFactor * originToTop) - eye_surface.y();
    eye_to_bottom = (scaleFactor * originToBottom) + eye_surface.y();

    // Distances in near plane, in near plane from origin.  (Similar to above)
    // Find dists on near plane using similar triangles
    const double zNear = 0.75;
    const double zFar = 15;
    const double near_dist_front = zNear / eye_to_screen; // constant factor
    const double n_eye_to_left   = eye_to_left * near_dist_front;
    const double n_eye_to_right  = eye_to_right * near_dist_front;
    const double n_eye_to_top    = eye_to_top * near_dist_front;
    const double n_eye_to_bottom = eye_to_bottom * near_dist_front;

    // Set frustum and calculate the matrix.
    osg::Matrix projectionMatrix;
    projectionMatrix.makeFrustum(-n_eye_to_left, n_eye_to_right,
                                 -n_eye_to_bottom,n_eye_to_top,
                                 zNear, zFar);
    return projectionMatrix;
}

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
    m_ScreenRotationAngleInDegrees = 120.0;
    double acuteAngleInRadians = osg::PI_2 - osg::inDegrees(m_ScreenRotationAngleInDegrees/2.0);

    m_Frustum[0].left = bezelWidth  * cos(acuteAngleInRadians);
    m_Frustum[0].right = (bezelWidth + screenWidth) * cos(acuteAngleInRadians);
    m_Frustum[0].bottom = screenHeight;
    m_Frustum[0].top = -screenHeight;
    m_Frustum[0].zNear =  0.75;
    m_Frustum[0].zFar =  100;

    m_Frustum[1].right = -bezelWidth  * cos(acuteAngleInRadians);
    m_Frustum[1].left = -(bezelWidth + screenWidth) * cos(acuteAngleInRadians);
    m_Frustum[1].bottom = screenHeight;
    m_Frustum[1].top = -screenHeight;
    m_Frustum[1].zNear =  0.75;
    m_Frustum[1].zFar =  100;
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
    
    unsigned int numCameras = 2;

    double angleInRadians = osg::inDegrees(m_ScreenRotationAngleInDegrees);

    osg::Camera* viewCamera = getCamera();

    for(unsigned int i=0; i<numCameras;++i)
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

        osg::Matrixd cameraMatrix;
        //cameraMatrix.makePerspective(m_ScreenRotationAngleInDegrees / aspectRatio, aspectRatio, 0.1, 2.0);
        viewCamera->setProjectionMatrix(cameraMatrix);


        osg::ref_ptr<osg::Camera> camera = new osg::Camera();
        camera->setName(i ? leftTvName : rightTvName);
        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
        camera->setGraphicsContext(gc);
        camera->setViewport(new osg::Viewport(0,0, resolution.width, resolution.height));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);

        double acuteAngle = (osg::PI_2 - angleInRadians/2.0);
        osg::Matrixd viewMatrix, projectionMatrix;

        if (camera->getName() == rightTvName)
        {
            projectionMatrix.makeFrustum(m_Frustum[i].left, m_Frustum[i].right, m_Frustum[i].top, m_Frustum[i].bottom, m_Frustum[i].zNear, m_Frustum[i].zFar);
            viewMatrix = osg::Matrix::rotate(acuteAngle/2.0, osg::Vec3(0,1,0)) * viewMatrix;

#ifdef SHEAR_RIGHT
            double s_z = cos(acuteAngle);
            osg::Matrixd shearMatrix(   1.0, 0.0, 0.0, 0.0,\
                                        0.0, 1.0, 0.0, 0.0,\
                                        -s_z, 0.0, 1.0, 0.0,\
                                        0.0, 0.0, 0.0, 1.0);
            projectionMatrix =  shearMatrix * projectionMatrix;
#endif
        }
        else
        {
            projectionMatrix.makeFrustum(m_Frustum[i].left, m_Frustum[i].right, m_Frustum[i].top, m_Frustum[i].bottom, m_Frustum[i].zNear, m_Frustum[i].zFar);
            viewMatrix = osg::Matrix::rotate(-acuteAngle/2.0, osg::Vec3(0,1,0)) * viewMatrix;
#ifdef SHEAR_LEFT
            double s_z = cos(acuteAngle);
            osg::Matrixd shearMatrix(   1.0, 0.0, 0.0, 0.0,\
                                        0.0, 1.0, 0.0, 0.0,\
                                        s_z, 0.0, 1.0, 0.0,\
                                        0.0, 0.0, 0.0, 1.0);
            projectionMatrix = shearMatrix * projectionMatrix;
#endif
        }

        viewCamera->setViewMatrix(osg::Matrixd());

        addSlave(camera.get(), projectionMatrix, viewMatrix);
    }
}

void MultiViewerWidget::SetStereoSettings()
{
    m_displaySettings->setStereo(true);
    m_displaySettings->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    m_displaySettings->setDisplayType(osg::DisplaySettings::MONITOR);
    m_displaySettings->setScreenDistance(screenDistance); 
    m_displaySettings->setScreenHeight(screenHeight);
    m_displaySettings->setScreenWidth(screenWidth);
    m_displaySettings->setEyeSeparation(0.06f);
}

void MultiViewerWidget::paintEvent( QPaintEvent* event )
{ 
    frame(); 
}