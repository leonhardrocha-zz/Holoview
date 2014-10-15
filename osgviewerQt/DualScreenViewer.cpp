#include "DualScreenViewer.h"
#include "TrackerManipulator.h"
#include "JoystickManipulator.h"
#include "MatrixExtension.h"
#include "ICallable.h"
#include "IArgs.h"
#include <math.h>
#include <iostream>

template<class matrix_type, class value_type>
bool _myclampProjectionMatrix(matrix_type& projection, double& znear, double& zfar, value_type delta_span)
{
    double epsilon = 1e-6;
    if (zfar<znear-epsilon)
    {
        if (zfar != -FLT_MAX || znear != FLT_MAX)
        {
            OSG_INFO<<"_clampProjectionMatrix not applied, invalid depth range, znear = "<<znear<<"  zfar = "<<zfar<<std::endl;
        }
        return false;
    }

    if (zfar<znear+epsilon)
    {
        // znear and zfar are too close together and could cause divide by zero problems
        // late on in the clamping code, so move the znear and zfar apart.
        double average = (znear+zfar)*0.5;
        znear = average-epsilon;
        zfar = average+epsilon;
        // OSG_INFO << "_clampProjectionMatrix widening znear and zfar to "<<znear<<" "<<zfar<<std::endl;
    }

    if (fabs(projection(0,3))<epsilon  && fabs(projection(1,3))<epsilon  && fabs(projection(2,3))<epsilon )
    {
        // OSG_INFO << "Orthographic matrix before clamping"<<projection<<std::endl;

        //value_type delta_span = (zfar-znear)*0.02;
        if (delta_span<1.0) delta_span = 1.0;
        value_type desired_znear = znear - delta_span;
        value_type desired_zfar = zfar + delta_span;

        // assign the clamped values back to the computed values.
        znear = desired_znear;
        zfar = desired_zfar;

        projection(2,2)=-2.0f/(desired_zfar-desired_znear);
        projection(3,2)=-(desired_zfar+desired_znear)/(desired_zfar-desired_znear);

        // OSG_INFO << "Orthographic matrix after clamping "<<projection<<std::endl;
    }
    else
    {
        // OSG_INFO << "Persepective matrix before clamping"<<projection<<std::endl;

        //std::cout << "_computed_znear"<<_computed_znear<<std::endl;
        //std::cout << "_computed_zfar"<<_computed_zfar<<std::endl;

        //value_type zfarPushRatio = 1.02;
        //value_type znearPullRatio = 0.98;

        //znearPullRatio = 0.99;

        /*value_type desired_znear = znear * znearPullRatio;
        value_type desired_zfar = zfar * zfarPushRatio;*/

        value_type desired_znear = znear + delta_span;
        value_type desired_zfar = zfar + delta_span;

        // near plane clamping.
        /*double min_near_plane = zfar*nearFarRatio;
        if (desired_znear<min_near_plane) desired_znear=min_near_plane;*/

        // assign the clamped values back to the computed values.
        znear = desired_znear;
        zfar = desired_zfar;

        value_type trans_near_plane = (-desired_znear*projection(2,2)+projection(3,2))/(-desired_znear*projection(2,3)+projection(3,3));
        value_type trans_far_plane = (-desired_zfar*projection(2,2)+projection(3,2))/(-desired_zfar*projection(2,3)+projection(3,3));

        value_type ratio = fabs(2.0/(trans_near_plane-trans_far_plane));
        value_type center = -(trans_near_plane+trans_far_plane)/2.0;

        projection.postMult(osg::Matrix(1.0f,0.0f,0.0f,0.0f,
                                        0.0f,1.0f,0.0f,0.0f,
                                        0.0f,0.0f,ratio,0.0f,
                                        0.0f,0.0f,center*ratio,1.0f));

        // OSG_INFO << "Persepective matrix after clamping"<<projection<<std::endl;
    }
    return true;
}

struct MyClampProjectionMatrixCallback : public osg::CullSettings::ClampProjectionMatrixCallback
{
    MyClampProjectionMatrixCallback(double deltaSpan) : _deltaSpan(deltaSpan)
    {}

    virtual bool clampProjectionMatrixImplementation(osg::Matrixf& projection, double& znear, double& zfar) const
    {
        return _myclampProjectionMatrix( projection, znear, zfar, _deltaSpan );
    };
    virtual bool clampProjectionMatrixImplementation(osg::Matrixd& projection, double& znear, double& zfar) const
    {
        return _myclampProjectionMatrix( projection, znear, zfar, _deltaSpan );
    };

    double _deltaSpan;
};


class ViewUpdateHandler : public osgGA::GUIEventHandler, public ICallable
{
public:
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) 
    { 
        Call();
        return osgGA::GUIEventHandler::handle(ea,aa);
    }

    virtual void SetCallback(ICallback callback, void* instance=NULL, IArgs* args=NULL)
    {
        m_callback = callback;
        m_instance = instance;
        m_callbackArgs = args;
    };

    virtual void Call() { if (m_callback) { (*m_callback)(m_instance, m_callbackArgs); } else throw new std::exception("Callback is not set or NULL");};

    virtual ICallback GetCallback() { return m_callback;};
    virtual IArgs* GetArgs() { return m_callbackArgs;};
    virtual void* GetInstance() { return m_instance;};
protected:

    ICallback               m_callback;
    void*                   m_instance;
    IArgs*                  m_callbackArgs;
    osg::ref_ptr<DualScreenViewer> m_parent;
};


DualScreenViewer::DualScreenViewer() : osgViewer::CompositeViewer()
{
    m_angleBetweenScreensInDegrees = 120;
    // Create View 0 -- Main.
    {
        osgViewer::View* view = new osgViewer::View;
        osg::DisplaySettings* ds = new osg::DisplaySettings();
        view->setDisplaySettings(ds);
        CreateGraphicsWindow(view);
        ToggleStereoSettings(view);
        ViewUpdateHandler* viewUpdateHandler = new  ViewUpdateHandler();
        viewUpdateHandler->SetCallback(DualScreenViewer::UpdateMap, this, &m_viewerArgs);
        view->addEventHandler( viewUpdateHandler );

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
        m_viewerArgs.Set(view->getName(), view);

        //view->getCamera()->setClampProjectionMatrixCallback( dynamic_cast<osg::CullSettings::ClampProjectionMatrixCallback*>(new MyClampProjectionMatrixCallback(-m_virtualCenter.z()) ));

        addView( view );
    }

    // Create view 1 -- Map.
    {
        osgViewer::View* view = new osgViewer::View;
        osg::DisplaySettings* ds = new osg::DisplaySettings();
        view->setDisplaySettings(ds);
        view->setUpViewInWindow( 10, 510, 640, 480 );

        osg::ref_ptr<osgGA::TrackballManipulator> mouseManipulator = new osgGA::TrackballManipulator;
        mouseManipulator->setName("MapMouse");
        view->setCameraManipulator( mouseManipulator );
        view->setName("map");
        m_viewerArgs.Set(view->getName(), view);
        addView( view );
    }

    m_traits = new osg::GraphicsContext::Traits;

#ifndef WIN32
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
#endif

    setKeyEventSetsDone(0);
    osgViewer::View* mainView = static_cast<osgViewer::View*>(m_viewerArgs.Get("main"));
    m_traits->windowName = mainView->getName();
    osg::DisplaySettings* ds = mainView->getDisplaySettings();
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
}

DualScreenViewer::~DualScreenViewer() 
{
}

void DualScreenViewer::Setup()
{
    m_virtualOrigin = osg::Vec3(0.0,    m_display.Elevation + m_display.Height/2,   0.0);
    m_virtualCenter = osg::Vec3(0.0,    m_virtualOrigin.y(),                           m_display.screenDepth);
    SetupView();
    SetupProjection();
}

void DualScreenViewer::Update(IArgs *results) 
{
    osgViewer::View* view = static_cast<osgViewer::View*>(m_viewerArgs.Get("main"));
    osgGA::KeySwitchMatrixManipulator* keyManipulator = static_cast<osgGA::KeySwitchMatrixManipulator*>(view->getCameraManipulator());
    if (keyManipulator)
    {
        osgGA::CameraManipulator* cameraManipulator = keyManipulator->getCurrentMatrixManipulator(); //keyManipulator->getMatrixManipulatorWithIndex(i);
        HandleManipulator(cameraManipulator, results);
    }
    Update(view, m_virtualEye);
}

void DualScreenViewer::HandleManipulator(osgGA::CameraManipulator* cameraManipulator, IArgs *results)
{
    osg::Quat rotation;
    osg::Vec3d eye;

    if (cameraManipulator->getName() == "Tracker")
    {
        osgGA::TrackerManipulator* trackerManipulator = static_cast<osgGA::TrackerManipulator*>(cameraManipulator);
        trackerManipulator->setTrackingResults(results, m_virtualCenter, m_virtualOrigin);
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

    m_virtualEye = eye;
}

void DualScreenViewer::UpdateMap(void* instance, IArgs* args)
{
    // Update the wireframe frustum
    DualScreenViewer* pThis = static_cast<DualScreenViewer*>(instance);
    osgViewer::View* mapView = static_cast<osgViewer::View*>(args->Get("map"));
    osg::Group* root = static_cast<osg::Group*>(mapView->getSceneData());
    if (root) 
    {
        osg::Node* map = root->getChild(1);
        osgViewer::View* mainView = static_cast<osgViewer::View*>(args->Get("main"));
        root->replaceChild(map, pThis->makeFrustumFromCamera( mainView ));
    }
}


void DualScreenViewer::SetupView()
{
    double angle = osg::inDegrees(m_angleBetweenScreensInDegrees/2.0);
    double angleOffset = osg::PI_2 - angle;
    double sx = tan(angleOffset);
    m_viewOffset[Right] = osg::Matrix::translate(-m_virtualCenter) * osg::Matrix::rotate(-angleOffset, osg::Vec3(0,1,0)) * MatrixExtension::getShear(osg::Vec3(sx, 0.0, 1.0)) * osg::Matrix::translate(m_virtualCenter);
    m_viewOffset[Left] = osg::Matrix::translate(-m_virtualCenter) * osg::Matrix::rotate(angleOffset, osg::Vec3(0,1,0)) * MatrixExtension::getShear(osg::Vec3(-sx, 0.0, 1.0)) * osg::Matrix::translate(m_virtualCenter);;
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
    double offsetX = eye.x()/m_display.screenWidth;
    double offsetY = eye.y()/m_display.screenHeight;
    double offsetZ = (eye.z()-m_virtualCenter.z())/m_display.screenDepth;

    osg::DisplaySettings* ds = view->getDisplaySettings();
    ds->setScreenDistance(-eye.length());

    for (int i = 0; i < NumOfScreens; i++)
    {
        osgViewer::View::Slave& slave = view->getSlave(i);
        //slave._viewOffset = m_viewOffset[i];
        slave._projectionOffset = m_projectionOffset[i] * osg::Matrix::translate(offsetX, offsetY, offsetZ);
    }
}


void DualScreenViewer::CreateGraphicsWindow(osgViewer::View* view)
{
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi) 
    {
        osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
        return;
    }
    Setup();
    osg::Camera* viewCamera = view->getCamera();
    viewCamera->setViewMatrix(m_viewMatrix);
    viewCamera->setProjectionMatrix(m_projectionMatrix);
    for(unsigned int i=0; i < NumOfScreens; ++i)
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
        camera->setName(i==Right ? "Right" : "Left");
        if (view->addSlave(camera.get(), m_projectionOffset[i], m_viewOffset[i]))
        {
            osg::View::Slave& slave = view->getSlave(i);
            slave._updateSlaveCallback = new DualScreenViewer::SlaveCallback();
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


osg::Geode* DualScreenViewer::drawFrustum(const osg::Matrixd& proj)
{
    //    // Get near and far from the Projection matrix.

    //// Get the sides of the near plane.
    double fLeft, fRight, fBottom, fTop;
    // Get the sides of the far plane.
    double nLeft, nRight, nBottom, nTop;

    double zNear, zFar;

    if (!proj.getFrustum(nLeft, nRight, nBottom, nTop, zNear, zFar))
    {
        zNear = proj(3,2) / (proj(2,2)-1.0);
        zFar = proj(3,2) / (1.0+proj(2,2));

        nLeft = zNear * (proj(2,0)-1.0) / proj(0,0);
        nRight = zNear * (1.0+proj(2,0)) / proj(0,0);
        nTop = zNear * (1.0+proj(2,1)) / proj(1,1);
        nBottom = zNear * (proj(2,1)-1.0) / proj(1,1);

        fLeft = zFar * (proj(2,0)-1.0) / proj(0,0);
        fRight = zFar * (1.0+proj(2,0)) / proj(0,0);
        fTop = zFar * (1.0+proj(2,1)) / proj(1,1);
        fBottom = zFar * (proj(2,1)-1.0) / proj(1,1);
    }
    else
    {
        double ratio = zFar/zNear;
        fLeft = nLeft * ratio;
        fRight = nRight * ratio;
        fTop = nTop * ratio;
        fBottom = nBottom * ratio;
    }

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

    osg::Geode* geode = new osg::Geode;
    geode->addDrawable( geom );

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
        mt->addChild( drawFrustum(proj) );
    }
    for(unsigned int i = 0; i < numOfSlaves; i++)
    {
        osg::View::Slave& slave = view->getSlave(i);
        osg::Matrixd slaveProj(proj * m_projectionOffset[i]);
        osg::MatrixTransform* mtSlave = new osg::MatrixTransform;
        mtSlave->setMatrix( osg::Matrixd::inverse( slave._viewOffset ) );
        mtSlave->addChild( drawFrustum(slaveProj) );
        mt->addChild( mtSlave );
    }
    return mt;
}
