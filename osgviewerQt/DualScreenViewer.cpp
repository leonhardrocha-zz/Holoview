#include "DualScreenViewer.h"
#include <math.h>
#include <iostream>
#ifdef _DEBUG
#include "vld.h"
#endif


DualScreenViewer::DualScreenViewer() : osgViewer::Viewer()
{
    setDisplaySettings(osg::DisplaySettings::instance().get());
    m_displaySettings = getDisplaySettings();
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
    m_screenDistance = 1.0; // 1.0 m
    m_screenDepth = 30.0 ; // 3.0 m
    m_angleBetweenScreensInDegrees = 120.0;
    m_tvElevation = 0.75;

    double rotationAngle = osg::PI_2 - osg::inDegrees(m_angleBetweenScreensInDegrees/2.0);
    double cosine = cos(rotationAngle);
    double sine = sin(rotationAngle);
    double fovx = m_angleBetweenScreensInDegrees /2.0;
    double aspectRatio = (double) m_screenWidth / (double) m_screenHeight;

    m_screen[Right].name = "RightTV";
    m_screen[Right].left =  m_bezelWidth  * cosine;
    m_screen[Right].right = (m_bezelWidth + m_screenWidth) * cosine;
    m_screen[Right].bottom = m_tvElevation + m_bezelWidth;
    m_screen[Right].top = m_tvElevation + m_bezelWidth + m_screenHeight;
    m_screen[Right].zLeft =  m_bezelWidth * sine;
    m_screen[Right].zRight =  (m_bezelWidth + m_screenWidth) * sine;
    m_screen[Right].center = osg::Vec3((m_screen[Right].left + m_screen[Right].right)/2.0, (m_screen[Right].top + m_screen[Right].bottom)/2.0, (m_screen[Right].zLeft + m_screen[Right].zRight)/2.0);

    m_screen[Left].name = "LeftTV";
    m_screen[Left].right = -m_bezelWidth  * cosine;
    m_screen[Left].left = -(m_bezelWidth + m_screenWidth) * cosine;
    m_screen[Left].bottom = m_tvElevation + m_bezelWidth;
    m_screen[Left].top = m_tvElevation  + m_bezelWidth + m_screenHeight;
    m_screen[Left].zLeft =  (m_bezelWidth + m_screenWidth) * sine;
    m_screen[Left].zRight = m_bezelWidth * sine;
    m_screen[Left].center = osg::Vec3((m_screen[Left].left + m_screen[Left].right)/2.0, (m_screen[Left].top + m_screen[Left].bottom)/2.0, (m_screen[Left].zLeft + m_screen[Left].zRight)/2.0);

    m_rightTVBasePosition = osg::Vec3(m_tvWidth / 2.0 * cosine, m_tvElevation, m_tvWidth /2.0 * sine);
    m_leftTVBasePosition = osg::Vec3(-m_tvWidth / 2.0 * cosine, m_tvElevation, m_tvWidth /2.0 * sine);
    m_virtualOrigin = osg::Vec3(0,m_tvElevation + m_tvHeight/2.0, 0);
    m_virtualCenter = m_virtualOrigin + osg::Vec3(0.0, 0.0, m_tvWidth * sine);

    m_projectionMatrix.makePerspective(fovx / aspectRatio, aspectRatio, 0.01, m_screenDistance + m_screenDepth);
    m_projectionMatrixOffset[Left] = osg::Matrix::translate(osg::Vec3(-m_tvWidth, 0, 0));
    m_projectionMatrixOffset[Right] =  osg::Matrix::translate(osg::Vec3(m_tvWidth, 0, 0));

    m_viewMatrix.makeLookAt(m_virtualCenter, m_virtualOrigin, osg::Vec3(0,1,0));
    m_eyeOffset = m_virtualCenter - m_virtualOrigin;
    m_displaySettings->setScreenDistance(m_eyeOffset.length());

    for(int i = 0; i < NumOfScreens; i++)
    {
        osg::Vec3 pa(m_screen[i].left, m_screen[i].bottom, m_screen[i].zLeft);
        osg::Vec3 pb(m_screen[i].right, m_screen[i].bottom, m_screen[i].zRight);
        osg::Vec3 pc(m_screen[i].left, m_screen[i].top, m_screen[i].zLeft);
        osg::Vec3 vr = pb - pa;
        osg::Vec3 vu = pc - pa;
        osg::Vec3 vn = vr ^ vu;
        vr.normalize();
        vu.normalize();
        vn.normalize();
        osg::Matrix viewMatrix(vr.x(), vr.y(), vr.z(), 0.0,
                               vu.x(), vu.y(), vu.z(), 0.0,
                               vn.x(), vn.y(), vn.z(), 0.0,
                              0.0,0.0,0.0, 1.0);
        m_viewMatrixOffset[i] = viewMatrix.translate(-m_eyeOffset);
    }
    
}

DualScreenViewer::~DualScreenViewer() 
{
}

void DualScreenViewer::UpdateEyeOffset(osg::Vec3 eyePosition) 
{
    m_eyeOffset = eyePosition - m_virtualCenter;
    m_viewMatrix.lookAt(eyePosition, m_virtualCenter, osg::Vec3(0,1,0));
    m_inverseAttitude = m_viewMatrix.getRotate().inverse();

    //for(int i = 0; i < NumOfScreens; i++)
    //{
    //    osg::View::Slave& slave = getSlave(i);
    //    slave._viewOffset = m_viewMatrixOffset[i].translate(-m_eyeOffset);
    //}

}

void DualScreenViewer::CreateGraphicsWindow()
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }

    osg::Camera* viewCamera = getCamera();

    for(unsigned int i=Left; i < NumOfScreens; ++i)
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
        traits->width = resolution.width / (NumOfScreens - 1);
        traits->height = resolution.height;
        traits->windowDecoration = false;
        traits->doubleBuffer = true;
        traits->sharedContext = 0;

        osg::ref_ptr<osg::Camera> camera = new osg::Camera();
        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
        camera->setGraphicsContext(gc);
        camera->setViewport(new osg::Viewport(0,0, resolution.width, resolution.height));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);
        camera->setName(i == Left ? "Left" : "Right");
        viewCamera->setViewMatrix(m_viewMatrix);
        viewCamera->setProjectionMatrix(m_projectionMatrix);
        addSlave(camera.get(), m_projectionMatrixOffset[i], m_viewMatrixOffset[i]);
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