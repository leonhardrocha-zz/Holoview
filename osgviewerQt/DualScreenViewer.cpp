#include "DualScreenViewer.h"
#include <math.h>
#include <iostream>
#ifdef _DEBUG
#include "vld.h"
#endif


DualScreenViewer::DualScreenViewer() : osgViewer::Viewer()
{
    m_displaySettings = osg::DisplaySettings::instance().get();
    m_traits = new osg::GraphicsContext::Traits;

#ifndef WIN32
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
#endif

    setKeyEventSetsDone(0);
    m_traits->windowName = getName();
    m_traits->windowDecoration = false;
    m_traits->x = 0;
    m_traits->y = 0;
    m_traits->width = m_displaySettings->getScreenWidth();
    m_traits->height = m_displaySettings->getScreenHeight();
    m_traits->doubleBuffer = true;
    m_traits->alpha = m_displaySettings->getMinimumNumAlphaBits();
    m_traits->stencil = m_displaySettings->getMinimumNumStencilBits();
    m_traits->sampleBuffers = m_displaySettings->getMultiSamples();
    m_traits->samples = m_displaySettings->getNumMultiSamples();


    m_tvWidth = 60 * 0.0254; // 60 inches
    m_tvHeight = 36 * 0.0254; // 36 inches 
    m_tvDepth = 1.5 * 0.0254; // 1.5 inches
    m_bezelWidth = 2 * 0.0254; // 2 inches
    m_bezelHeight = 2 * 0.0254; //2 inches
    m_screenWidth = 56 * 0.0254; // 56 inches
    m_screenHeight = 32 * 0.0254; // 32 inches
    m_screenDistance = 50 * 0.0254; // 50 inches
    m_screenDepth = 1.0 ; // 0
    m_angleBetweenScreenInDegrees = 120.0;

    double rotationAngleInRadians = osg::PI_2 - osg::inDegrees(m_angleBetweenScreenInDegrees/2.0);

    rightTvName = "RightTV";
    m_Frustum[0].left =  m_bezelWidth  * cos(rotationAngleInRadians);
    m_Frustum[0].right = (m_bezelWidth + m_screenWidth) * cos(rotationAngleInRadians);
    m_Frustum[0].bottom = m_screenHeight;
    m_Frustum[0].top = -m_screenHeight;
    m_Frustum[0].zNear =  .25;
    m_Frustum[0].zFar =  2;

    leftTvName = "LeftTV";
    m_Frustum[1].right = (m_bezelWidth + m_screenWidth) * cos(rotationAngleInRadians);/*-m_bezelWidth  * cos(rotationAngleInRadians);*/
    m_Frustum[1].left = -(m_bezelWidth + m_screenWidth) * cos(rotationAngleInRadians);
    m_Frustum[1].bottom = m_screenHeight;
    m_Frustum[1].top = -m_screenHeight;
    m_Frustum[1].zNear =  0.25;
    m_Frustum[1].zFar =  2;
}

DualScreenViewer::~DualScreenViewer() 
{
}


void DualScreenViewer::CreateGraphicsWindow()
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }
    
    unsigned int numCameras = 2;

    double angleInRadians = osg::inDegrees(m_angleBetweenScreenInDegrees);

    osg::Camera* viewCamera = getCamera();

    for(unsigned int i=0; i<numCameras;++i)
    {
        osg::GraphicsContext::ScreenIdentifier screenId = osg::GraphicsContext::ScreenIdentifier(i);
        screenId.setUndefinedScreenDetailsToDefaultScreen();
        screenId.readDISPLAY();
        osg::GraphicsContext::ScreenSettings resolution;
        wsi->getScreenSettings(screenId, resolution);
        double aspectRatio = (double)resolution.width/(double)resolution.height;

        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->screenNum = screenId.screenNum;
        traits->x = 0;
        traits->y = 0;
        traits->width = resolution.width / (numCameras - 1);
        traits->height = resolution.height;
        traits->windowDecoration = false;
        traits->doubleBuffer = true;
        traits->sharedContext = 0;

        osg::ref_ptr<osg::Camera> camera = new osg::Camera();
        camera->setName(i ? leftTvName : rightTvName);
        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
        camera->setGraphicsContext(gc);
        camera->setViewport(new osg::Viewport(0,0, resolution.width, resolution.height));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);

        double acuteAngle = (osg::PI_2 - angleInRadians/2.0);
        double fovx = m_angleBetweenScreenInDegrees /2.0;
        osg::Matrixd viewMatrix, projectionMatrix;
        projectionMatrix.makePerspective(fovx / aspectRatio, aspectRatio, 0.1, 1.75);
        if (camera->getName() == rightTvName)
        {
            //projectionMatrix.makeFrustum(m_Frustum[i].left, m_Frustum[i].right, m_Frustum[i].top, m_Frustum[i].bottom, m_Frustum[i].zNear, m_Frustum[i].zFar);
            projectionMatrix = osg::Matrix::translate(osg::Vec3(0,0,-1)) * osg::Matrix::rotate(acuteAngle, osg::Vec3(0,1,0)) * projectionMatrix;
            /*viewMatrix = osg::Matrix::rotate(acuteAngle/2.0, osg::Vec3(0,1,0));*/
        }
        else
        {
            /*projectionMatrix.makeFrustum(m_Frustum[i].left, m_Frustum[i].right, m_Frustum[i].top, m_Frustum[i].bottom, m_Frustum[i].zNear, m_Frustum[i].zFar);*/
            projectionMatrix = osg::Matrix::translate(osg::Vec3(0,0,-1)) * osg::Matrix::rotate(-acuteAngle, osg::Vec3(0,1,0)) * projectionMatrix;
            /*viewMatrix = osg::Matrix::rotate(-acuteAngle/2.0, osg::Vec3(0,1,0));*/
        }

        viewCamera->setViewMatrix(osg::Matrixd());
        viewCamera->setProjectionMatrix(osg::Matrixd());
        addSlave(camera.get(), projectionMatrix, viewMatrix);
    }
}

void DualScreenViewer::SetStereoSettings()
{
    m_displaySettings->setStereo(true);
    m_displaySettings->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    m_displaySettings->setDisplayType(osg::DisplaySettings::MONITOR);
    m_displaySettings->setScreenDistance(m_screenDistance); 
    m_displaySettings->setScreenHeight(m_screenHeight);
    m_displaySettings->setScreenWidth(m_screenWidth);
    m_displaySettings->setEyeSeparation(0.06f);
}