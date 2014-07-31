#include "DualScreenViewer.h"
#include "MatrixExtension.h"
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

    m_screen[Right].name = "Right";
    m_screen[Right].left =  m_bezelWidth  * cosine;
    m_screen[Right].right = (m_bezelWidth + m_screenWidth) * cosine;
    m_screen[Right].bottom = m_tvElevation + m_bezelWidth;
    m_screen[Right].top = m_tvElevation + m_bezelWidth + m_screenHeight;
    m_screen[Right].zFar = m_bezelWidth * sine;
    m_screen[Right].zNear = (m_bezelWidth + m_screenWidth) * sine;
    m_screen[Right].zRight = m_screen[Right].zNear;
    m_screen[Right].zLeft = m_screen[Right].zFar;
    m_screen[Right].center = osg::Vec3((m_screen[Right].left + m_screen[Right].right)/2.0, (m_screen[Right].top + m_screen[Right].bottom)/2.0, (m_screen[Right].zLeft + m_screen[Right].zRight)/2.0);

    m_screen[Left].name = "Left";
    m_screen[Left].right = -m_bezelWidth  * cosine;
    m_screen[Left].left = -(m_bezelWidth + m_screenWidth) * cosine;
    m_screen[Left].bottom = m_tvElevation + m_bezelWidth;
    m_screen[Left].top = m_tvElevation  + m_bezelWidth + m_screenHeight;
    m_screen[Left].zFar = m_bezelWidth * sine;
    m_screen[Left].zNear = (m_bezelWidth + m_screenWidth) * sine;
    m_screen[Left].zRight = m_screen[Left].zFar;
    m_screen[Left].zLeft = m_screen[Left].zNear;
    m_screen[Left].center = osg::Vec3((m_screen[Left].left + m_screen[Left].right)/2.0, (m_screen[Left].top + m_screen[Left].bottom)/2.0, (m_screen[Left].zLeft + m_screen[Left].zRight)/2.0);

    m_rightTVBasePosition = osg::Vec3(m_tvWidth / 2.0 * cosine, m_tvElevation, m_tvWidth /2.0 * sine);
    m_leftTVBasePosition = osg::Vec3(-m_tvWidth / 2.0 * cosine, m_tvElevation, m_tvWidth /2.0 * sine);
    m_virtualOrigin = osg::Vec3(0,m_tvElevation + m_tvHeight/2.0, 0);
    m_virtualCenter = m_virtualOrigin + osg::Vec3(0.0, 0.0, m_tvWidth * sine);

    m_viewMatrix.makeLookAt(m_virtualCenter, m_virtualOrigin, osg::Vec3(0,1,0));
    m_eyeOffset = m_virtualCenter - m_virtualOrigin;
    m_displaySettings->setScreenDistance(m_eyeOffset.length());
    osg::Matrix screenMatrix[2];
    for(int i = Left; i < NumOfScreens; i++)
    {
        osg::Vec3 pa(m_screen[i].left, m_screen[i].bottom, m_screen[i].zLeft);
        osg::Vec3 pb(m_screen[i].right, m_screen[i].bottom, m_screen[i].zRight);
        osg::Vec3 pc(m_screen[i].left, m_screen[i].top, m_screen[i].zLeft);
        osg::Vec3 pe(i == Left ? m_screen[i].right : m_screen[i].left, m_screen[i].top, m_screen[i].zLeft);
        osg::Vec3 vr = pb - pa;
        osg::Vec3 vu = pc - pa;
        osg::Vec3 vn = vr ^ vu;
        vr.normalize();
        vu.normalize();
        vn.normalize();
        screenMatrix[i].set   (vr.x(), vr.y(), vr.z(), 0.0,
                               vu.x(), vu.y(), vu.z(), 0.0,
                               vn.x(), vn.y(), vn.z(), 0.0,
                               0.0, 0.0, 0.0, 1.0);
        screenMatrix[i].preMultTranslate(-(m_eyeOffset));
        m_viewMatrixOffset[i] = screenMatrix[i];
    }

    double left, right, top, bottom, zNear, zFar, zRatio;

    zNear =  m_virtualCenter.z() - m_screen[Left].zNear;
    zFar = m_virtualCenter.z() - m_screen[Left].zFar;
    zRatio = zNear / zFar;
    right = (m_screen[Left].right - m_virtualCenter.x()) * zRatio;
    left = -(m_virtualCenter.x() - m_screen[Left].left)* zRatio;
    top = (m_screen[Left].top  - m_virtualCenter.y() )* zRatio;
    bottom = -(m_virtualCenter.y()  - m_screen[Left].bottom) * zRatio;
    m_projectionMatrixOffset[Right] =  osg::Matrix::frustum(left, right, bottom, top, zNear, zFar);

    zNear = m_virtualCenter.z() - m_screen[Right].zNear;
    zFar = m_virtualCenter.z() - m_screen[Right].zFar;
    zRatio = zNear / zFar;
    right = (m_screen[Right].right - m_virtualCenter.x()) * zRatio;
    left = -(m_virtualCenter.x() - m_screen[Right].left)* zRatio;
    top = (m_screen[Right].top  - m_virtualCenter.y() )* zRatio;
    bottom = -(m_virtualCenter.y()  - m_screen[Right].bottom) * zRatio;
    m_projectionMatrixOffset[Left] = osg::Matrix::frustum(left, right, bottom, top, zNear, zFar);

}

DualScreenViewer::~DualScreenViewer() 
{
}

void DualScreenViewer::UpdateEyeOffset(osg::Vec3 eyePosition) 
{
    m_eyeOffset = eyePosition - m_virtualCenter;
    m_viewMatrix.lookAt(eyePosition, m_virtualCenter, osg::Vec3(0,1,0));
    m_inverseAttitude = m_viewMatrix.getRotate().inverse();
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
    viewCamera->setViewMatrix(m_viewMatrix);
    viewCamera->setProjectionMatrix(m_projectionMatrix);
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