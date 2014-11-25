#include "DualScreenViewer.h"
#include "TrackerManipulator.h"
#include "JoystickManipulator.h"
#include "MatrixExtension.h"
#include "ICallable.h"
#include "IArgs.h"
#include <math.h>
#include <iostream>
#include <algorithm> 


DualScreenViewer::DualScreenViewer(bool initialize, bool start) : osgViewer::CompositeViewer(), m_swapScreens(true), m_angleBetweenScreensInDegrees(120.0)
{
    if(initialize)
    {
        IsInitialized = Init();
    }
    if (IsInitialized && start)
    {
        IsRunning = true;
    }
}

DualScreenViewer::~DualScreenViewer() 
{
}

osgViewer::View* DualScreenViewer::GetView(ViewCompareFunction viewFun)
{
    osgViewer::ViewerBase::Views views;
    getViews(views);
    std::vector<osgViewer::View*>::iterator firstFound = std::find_if(views.begin(), views.end(), viewFun);
    return (*firstFound);
}

bool DualScreenViewer::InitViews()
{
    // Create View 0 -- Main.
    {
        osgViewer::View* view = new osgViewer::View();
        osg::DisplaySettings* ds = new osg::DisplaySettings();
        view->setDisplaySettings(ds);

        osg::ref_ptr<osgGA::TrackerManipulator> trackerManipulator = new osgGA::TrackerManipulator();
        trackerManipulator->setName("Tracker");
        trackerManipulator->setVerticalAxisFixed(false);

        osg::ref_ptr<osgGA::JoystickManipulator> joystickManipulator = new osgGA::JoystickManipulator();
        joystickManipulator->setName("Joystick");

        osg::ref_ptr<osgGA::TrackballManipulator> mouseManipulator = new osgGA::TrackballManipulator;
        mouseManipulator->setName("Mouse");

        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keySwitch = new osgGA::KeySwitchMatrixManipulator;
        keySwitch->addMatrixManipulator( '1', trackerManipulator->getName(), trackerManipulator );
        keySwitch->addMatrixManipulator( '2', joystickManipulator->getName(), joystickManipulator );
        keySwitch->addMatrixManipulator( '3', mouseManipulator->getName(), mouseManipulator );
        view->setCameraManipulator( keySwitch );

        view->setName("main");
        view->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
        addView( view );
    }

    // Create view 1 -- Map.
    {
        osgViewer::View* view = new osgViewer::View();
        osg::DisplaySettings* ds = new osg::DisplaySettings();
        view->setDisplaySettings(ds);

        osg::ref_ptr<osgGA::TrackballManipulator> mouseManipulator = new osgGA::TrackballManipulator;
        mouseManipulator->setName("MapMouse");
        view->setCameraManipulator( mouseManipulator );
        view->setName("map");
        addView( view );
    }

    return true;
}

bool DualScreenViewer::InitGraphics(osgViewer::View* view)
{
    if(!view)
    {
        return false;
    }

    m_traits = new osg::GraphicsContext::Traits;

#ifndef WIN32
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
#else
    setThreadingModel( osgViewer::ViewerBase::CullDrawThreadPerContext );
#endif

    setKeyEventSetsDone(0);

    m_traits->windowName = view->getName();
    osg::DisplaySettings* ds = view->getDisplaySettings();
    m_traits->width = ds->getScreenWidth();
    m_traits->height = ds->getScreenHeight();
    m_traits->doubleBuffer = true;
    m_traits->alpha = ds->getMinimumNumAlphaBits();
    m_traits->stencil = ds->getMinimumNumStencilBits();
    m_traits->sampleBuffers = ds->getMultiSamples();
    m_traits->samples = ds->getNumMultiSamples();
    m_traits->windowDecoration = true;
    m_traits->x = 0;
    m_traits->y = 0;

    return true;
}
bool DualScreenViewer::Init()
{
    m_virtualOrigin = osg::Vec3(0.0,    m_display.Elevation + m_display.Height/2,   0.0);
    m_virtualCenter = osg::Vec3(0.0,    m_virtualOrigin.y(),                        m_display.screenDepth);

    TiltedScreen rightDisplay(osg::Vec3( (m_display.BezelWidth                        ) , -m_display.screenHeight/2, m_virtualOrigin.z()),
                              osg::Vec3( (m_display.BezelWidth                        ) ,  m_display.screenHeight/2, m_virtualOrigin.z()),
                              osg::Vec3( (m_display.BezelWidth + m_display.screenWidth) , -m_display.screenHeight/2, m_virtualCenter.z()));
    TiltedScreen leftDisplay( osg::Vec3(-(m_display.BezelWidth + m_display.screenWidth) , -m_display.screenHeight/2, m_virtualCenter.z()),
                              osg::Vec3(-(m_display.BezelWidth + m_display.screenWidth) ,  m_display.screenHeight/2, m_virtualCenter.z()),
                              osg::Vec3(-(m_display.BezelWidth                        ) , -m_display.screenHeight/2, m_virtualOrigin.z()));
    m_displays.push_back(rightDisplay);
    m_displays.push_back(leftDisplay);

    SetupView();
    SetupProjection();

    if (InitViews())
    {
        osgViewer::View* mainView = GetMainView();
        //osgViewer::View* mapView = GetMapView();
        //setRunFrameScheme( osgViewer::ViewerBase::ON_DEMAND );
        if(InitGraphics(mainView))
        {
            CreateGraphicsWindow(mainView);
            ToggleStereoSettings(mainView);
            return true;
        }
    }

    return false;
}

void DualScreenViewer::Update() 
{
    osgViewer::View* view = GetMainView();
    osgGA::KeySwitchMatrixManipulator* keyManipulator = static_cast<osgGA::KeySwitchMatrixManipulator*>(view->getCameraManipulator());
    if (keyManipulator)
    {
        osg::Vec3d eye;
        osg::Quat rotation;
        osgGA::CameraManipulator* cameraManipulator = keyManipulator->getCurrentMatrixManipulator(); //keyManipulator->getMatrixManipulatorWithIndex(i);
        HandleManipulator(cameraManipulator, eye, rotation);
        Update(view, eye);
    }
}

void DualScreenViewer::HandleManipulator(osgGA::CameraManipulator* cameraManipulator, osg::Vec3d& eye, osg::Quat& rotation)
{
    if (cameraManipulator->getName() == "Tracker")
    {
        osgGA::TrackerManipulator* trackerManipulator = static_cast<osgGA::TrackerManipulator*>(cameraManipulator);
        trackerManipulator->setTrackingResults(m_virtualEye, m_virtualCenter);
        trackerManipulator->getTransformation(eye, rotation);
    }

    if (cameraManipulator->getName() == "Joystick")
    {
        osgGA::JoystickManipulator* joystickManipulator = static_cast<osgGA::JoystickManipulator*>(cameraManipulator);
        joystickManipulator->getTransformation(eye, rotation);
    }

    if (cameraManipulator->getName() == "Mouse")
    {
        osgGA::TrackballManipulator* trackballManipulator = static_cast<osgGA::TrackballManipulator*>(cameraManipulator);
        trackballManipulator->getTransformation(eye, rotation);
    }
}



void DualScreenViewer::SetupView()
{
    double angle = osg::inDegrees(m_angleBetweenScreensInDegrees/2.0);
    double angleOffset = osg::PI_2 - angle;
    double sx = tan(angleOffset);
    m_viewOffset[Right] = osg::Matrix::translate(0,0,-m_display.Depth) * osg::Matrix::rotate(-angleOffset, osg::Vec3(0,1,0)) * MatrixExtension::getShear(osg::Vec3( sx, 0.0, 1.0)) * osg::Matrix::translate(0,0,m_display.Depth);
    m_viewOffset[Left]  = osg::Matrix::translate(0,0,-m_display.Depth) * osg::Matrix::rotate( angleOffset, osg::Vec3(0,1,0)) * MatrixExtension::getShear(osg::Vec3(-sx, 0.0, 1.0)) * osg::Matrix::translate(0,0,m_display.Depth);
} 

void DualScreenViewer::SetupProjection()
{
    m_projectionMatrix = m_display.GetFrustum();
    double offset = 1.0 + m_display.BezelWidth/m_display.Width;
    m_projectionOffset[Right] = osg::Matrix::translate(offset, 0.0, 0.0);
    m_projectionOffset[Left]  = osg::Matrix::translate(-offset, 0.0, 0.0);
}

void DualScreenViewer::Update(osgViewer::View* view, osg::Vec3 eye)
{
    osg::Vec3 eyeOffset = eye-m_virtualOrigin;
    osg::Vec3 offset = eye-m_virtualCenter;
    //double offsetX = offset.x()/m_display.screenWidth;
    //double offsetY = offset.y()/m_display.screenHeight;
    //double offsetZ = offset.z()/m_display.screenDepth;

    osg::DisplaySettings* ds = view->getDisplaySettings();
    ds->setScreenDistance(-m_virtualCenter.length());
    //m_projectionMatrix = m_display.GetFrustum();
    //view->getCamera()->setProjectionMatrix(osg::Matrix::identity());

    for (int i = Right; i < NumOfScreens; i++)
    {
        m_displays[i].Update(eyeOffset);
        osgViewer::View::Slave& slave = view->getSlave(i);
        slave._viewOffset = m_displays[i].GetView();
        slave._projectionOffset = m_displays[i].GetFrustum();
    }
}

//bool addSlave(osg::View* view, osg::Camera* camera,  osg::Matrix& projectionOffset, osg::Matrix& viewOffset, osg::Matrix& postProjectionOffset, osg::Matrix& postViewOffset, bool useMastersSceneData=true)
//{
//    OsgExtension::View* extView = static_cast<OsgExtension::View*>(view);
//    if (extView)
//    {
//        extView->addSlave(camera, projectionOffset, viewOffset, postProjectionOffset, postViewOffset, useMastersSceneData);
//    } 
//    
//    return view->addSlave(camera, projectionOffset, viewOffset, useMastersSceneData);
//}


void DualScreenViewer::CreateGraphicsWindow(osgViewer::View* view)
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }
    osg::Camera* viewCamera = view->getCamera();
    viewCamera->setViewMatrix(m_viewMatrix);
    viewCamera->setProjectionMatrix(m_projectionMatrix);


    for(unsigned int j=0; j < NumOfScreens; ++j)
    {
        unsigned int i = m_swapScreens ? NumOfScreens -1 - j : j;
        osg::GraphicsContext::ScreenIdentifier screenId = osg::GraphicsContext::ScreenIdentifier(i);
        screenId.setUndefinedScreenDetailsToDefaultScreen();
        screenId.readDISPLAY();
        osg::GraphicsContext::ScreenSettings resolution;
        wsi->getScreenSettings(screenId, resolution);

        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->screenNum = screenId.screenNum;
        traits->width = resolution.width / (NumOfScreens - 1);
        traits->height = resolution.height;
        traits->x = 0;
        traits->y = 0;
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
        camera->setName(i==Right ? "Right" : "Left");
        if (view->addSlave(camera.get(), m_projectionOffset[i], m_viewOffset[i]))
        {
            osg::View::Slave& slave = view->getSlave(j);
            slave._updateSlaveCallback = new OsgExtension::SlaveCallback();
        }
    }
}

void DualScreenViewer::ToggleStereoSettings(osgViewer::View* view)
{
    osg::DisplaySettings* ds = view->getDisplaySettings();
    bool isStereo = ds->getStereo();
    ds->setStereo(!isStereo);
    if (ds->getStereo())
    {
        ds->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
        ds->setDisplayType(osg::DisplaySettings::MONITOR);
        ds->setScreenDistance(m_display.screenDepth); 
        ds->setScreenHeight(m_display.screenHeight);
        ds->setScreenWidth(m_display.screenWidth);
        ds->setEyeSeparation(0.05f);
    }
}


osg::Geometry* DualScreenViewer::GetFrustumGeometry(ScreenInfo& info)
{
    double nLeft = info.left;
    double nRight = info.right;
    double nTop = info.top;
    double nBottom = info.bottom;
    double zNear = info.zNear;
    double zFar = info.zFar;
    double ratio = zFar/zNear;
    double fLeft = nLeft * ratio;
    double fRight = nRight * ratio;
    double fTop = nTop * ratio;
    double fBottom = nBottom * ratio;
    

    // Our vertex array needs only 9 vertices: The origin, and the
    // eight corners of the near and far planes.
    osg::Vec3Array* v = new osg::Vec3Array;
    v->resize( 9 );
    (*v)[0].set( 0., 0., 0. );
    (*v)[1].set( nLeft, nBottom, -zNear );
    (*v)[2].set( nRight, nBottom, -zNear );
    (*v)[3].set( nRight, nTop, -zNear );
    (*v)[4].set( nLeft, nTop, -zNear );
    (*v)[5].set( fLeft, fBottom, -zFar );
    (*v)[6].set( fRight, fBottom, -zFar );
    (*v)[7].set( fRight, fTop, -zFar );
    (*v)[8].set( fLeft, fTop, -zFar );

    osg::Geometry* geom = new osg::Geometry;
    geom->setUseDisplayList( false );
    geom->setVertexArray( v );

    osg::Vec4Array* c = new osg::Vec4Array;
    c->push_back( osg::Vec4( 1., 1., 1., 1. ) );
    geom->setColorArray( c, osg::Array::BIND_OVERALL );

    GLushort idxLines[8] = {
        0, 5, 0, 6, 0, 7, 0, 8 };
    GLushort idxLoops0[4] = {
        1, 2, 3, 4 };
    GLushort idxLoops1[4] = {
        5, 6, 7, 8 };
    geom->addPrimitiveSet( new osg::DrawElementsUShort( osg::PrimitiveSet::LINES, 8, idxLines ) );
    geom->addPrimitiveSet( new osg::DrawElementsUShort( osg::PrimitiveSet::LINE_LOOP, 4, idxLoops0 ) );
    geom->addPrimitiveSet( new osg::DrawElementsUShort( osg::PrimitiveSet::LINE_LOOP, 4, idxLoops1 ) );

    return geom;
}

osg::Geode* DualScreenViewer::DrawFrustum(ScreenInfo& info)
{
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable( GetFrustumGeometry(info) );
    geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
    return geode;

}

osg::Geode* DualScreenViewer::DrawFrustum(TiltedScreen& info)
{
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable( GetFrustumGeometry(info) );
    geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
    return geode;
}

// Given a Camera, create a wireframe representation of its
// view frustum. Create a default representation if camera==NULL.
osg::MatrixTransform* DualScreenViewer::makeFrustumFromCamera(osgViewer::View* view)
{
    // Projection and ModelView matrices
    osg::Matrixd proj;
    osg::Matrixd mv;
    osg::Camera* camera = view->getCamera();
    if (camera)
    {
        proj = camera->getProjectionMatrix();
        mv = camera->getViewMatrix();
    }
    else
    {
        // Create some kind of reasonable default Projection matrix.
        proj.makePerspective( 30., 1., 1., 10. );
        // leave mv as identity
    }

     // Create parent MatrixTransform to transform the view volume by
    // the inverse ModelView matrix.
    osg::MatrixTransform* mt = new osg::MatrixTransform;
    mt->setMatrix( osg::Matrixd::inverse( mv ) );
    unsigned int numOfSlaves = view->getNumSlaves();
    if (numOfSlaves==0)
    {
        mt->addChild( DrawFrustum(m_display) );
    }

    for(unsigned int i = 0; i < numOfSlaves; i++)
    {
        osg::View::Slave& slave = view->getSlave(i);
        osg::MatrixTransform* mtSlave = new osg::MatrixTransform;
        mtSlave->setMatrix( osg::Matrixd::inverse( slave._viewOffset ) );
        mtSlave->addChild( DrawFrustum(m_displays[i]) );
        mt->addChild( mtSlave );
    }
    return mt;
}
