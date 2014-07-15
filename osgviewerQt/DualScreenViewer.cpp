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
    m_screenDistance = 1.0; // 1.0 m
    m_screenDepth = 30.0 ; // 3.0 m
    m_angleBetweenScreensInDegrees = 120.0;
    m_tvElevation = 0.75;

    double rotationAngle = osg::PI_2 - osg::inDegrees(m_angleBetweenScreensInDegrees/2.0);
    double cosine = cos(rotationAngle);
    double sine = sin(rotationAngle);

    rightTvName = "RightTV";
    m_Frustum[1].left =  m_bezelWidth  * cosine;
    m_Frustum[1].right = (m_bezelWidth + m_screenWidth) * cosine;
    m_Frustum[1].bottom = m_tvElevation + m_bezelWidth;
    m_Frustum[1].top = m_tvElevation + m_bezelWidth + m_screenHeight;
    m_Frustum[1].zFar =  m_bezelWidth * sine;
    m_Frustum[1].zNear =  (m_bezelWidth + m_screenWidth) * sine;

    leftTvName = "LeftTV";
    m_Frustum[0].right = -m_bezelWidth  * cosine;
    m_Frustum[0].left = -(m_bezelWidth + m_screenWidth) * cosine;
    m_Frustum[0].bottom = m_tvElevation + m_bezelWidth;
    m_Frustum[0].top = m_tvElevation  + m_bezelWidth + m_screenHeight;
    m_Frustum[0].zFar =  m_bezelWidth * sine;
    m_Frustum[0].zNear =  (m_bezelWidth + m_screenWidth) * sine;

    m_rightTVBasePosition = osg::Vec3(m_tvWidth / 2.0 * cosine, m_tvElevation, m_tvWidth /2.0 * sine);
    m_leftTVBasePosition = osg::Vec3(-m_tvWidth / 2.0 * cosine, m_tvElevation, m_tvWidth /2.0 * sine);

    double fovx = m_angleBetweenScreensInDegrees /2.0;
    double aspectRatio = (double) m_screenWidth / (double) m_screenHeight;

    m_projectionMatrix.makePerspective(fovx / aspectRatio, aspectRatio, m_screenDistance, m_screenDistance + m_screenDepth);
    m_leftProjectionOffsetMatrix= osg::Matrix::translate(osg::Vec3(0, 0,-m_screenDistance)) * osg::Matrix::rotate(-rotationAngle, osg::Vec3(0,1,0)) * m_projectionMatrix;
    m_rightProjectionOffsetMatrix= osg::Matrix::translate(osg::Vec3(0, 0,-m_screenDistance)) * osg::Matrix::rotate(rotationAngle, osg::Vec3(0,1,0)) * m_projectionMatrix;
}

DualScreenViewer::~DualScreenViewer() 
{
}


osg::Matrix DualScreenViewer::CalculateView(const osg::View::Slave& slave, const osg::Vec3 eyePos)
{
    int i = (slave._camera->getName() == "RightTV") ? 1 : 0;
    Frustum* pFrustum = &m_Frustum[i];
    osg::Vec3 pa(pFrustum->left, pFrustum->bottom, i ? pFrustum->zFar : pFrustum->zNear);
    osg::Vec3 pb(pFrustum->right, pFrustum->bottom, i ? pFrustum->zNear : pFrustum->zFar);
    osg::Vec3 pc(pFrustum->left, pFrustum->top, i ? pFrustum->zFar : pFrustum->zNear);
    osg::Vec3 pe(eyePos);

    osg::Vec3 vr = pb - pa;
    osg::Vec3 vu = pc - pa;
    vr.normalize();
    vu.normalize();
    osg::Vec3 vn = vr ^ vu;
    vn.normalize();

    osg::Vec3 va = pa - pe;
    osg::Vec3 vb = pb - pe;
    osg::Vec3 vc = pc - pe;
    
    //double zNear = m_screenDistance;
    //double zFar = m_screenDistance + m_screenDepth;
    //double d = -(va * vn);
    //double ratio = zNear / d;
    //double left = vr * va * ratio;
    //double right = vr * vb * ratio;
    //double bottom = vu * va * ratio;
    //double top = vu * vc * ratio;
    //
    //osg::Matrix f;
    //f.makeFrustum(left, right, bottom, top, zNear, zFar);

    osg::Matrix viewMatrix(vr.x(), vr.y(), vr.z(), 0.0,
                           vu.x(), vu.y(), vu.z(), 0.0,
                           vn.x(), vn.y(), vn.z(), 0.0,
                          -pe.x(),-pe.y(),-pe.z(), 1.0);
    double rotationAngle = osg::PI_2 - osg::inDegrees(m_angleBetweenScreensInDegrees/2.0);
    return osg::Matrix::rotate(i? rotationAngle : -rotationAngle, osg::Vec3(0,1,0)) * viewMatrix;
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

        osg::ref_ptr<osg::Camera> camera = new osg::Camera();
        camera->setName(i ? leftTvName : rightTvName);
        osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
        camera->setGraphicsContext(gc);
        camera->setViewport(new osg::Viewport(0,0, resolution.width, resolution.height));
        GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
        camera->setDrawBuffer(buffer);
        camera->setReadBuffer(buffer);
        osg::Matrix viewMatrixOffset;
        osg::Matrix projectionMatrixOffset;
        if (camera->getName() == rightTvName)
        {
            projectionMatrixOffset = m_rightProjectionOffsetMatrix;
        }
        else
        {
            projectionMatrixOffset = m_leftProjectionOffsetMatrix;
        }
        viewCamera->setViewMatrix(osg::Matrixd());
        viewCamera->setProjectionMatrix(osg::Matrixd());

        addSlave(camera.get(), projectionMatrixOffset, viewMatrixOffset);
        if (camera->getName() == rightTvName)
        {
            m_rightSlave = &getSlave(i);
        }
        else
        {
            m_leftSlave = &getSlave(i);
        }
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