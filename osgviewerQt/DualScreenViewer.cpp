#include "DualScreenViewer.h"
#include "DualScreenViewer.h"
#include "TrackerManipulator.h"
#include "JoystickManipulator.h"
#include "MatrixExtension.h"
#include "ICallable.h"
#include "IArgs.h"
#include <math.h>
#include <iostream>
#ifdef _DEBUG
#include "vld.h"
#endif



template<class matrix_type, class value_type>
bool _myclampProjectionMatrix(matrix_type& projection, double& znear, double& zfar, value_type nearFarRatio)
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

        value_type delta_span = (zfar-znear)*0.02;
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
        return true;
        // OSG_INFO << "Persepective matrix before clamping"<<projection<<std::endl;

        //std::cout << "_computed_znear"<<_computed_znear<<std::endl;
        //std::cout << "_computed_zfar"<<_computed_zfar<<std::endl;

        value_type zfarPushRatio = 1.02;
        value_type znearPullRatio = 0.98;

        //znearPullRatio = 0.99;

        value_type desired_znear = znear * znearPullRatio;
        value_type desired_zfar = zfar * zfarPushRatio;

        // near plane clamping.
        double min_near_plane = zfar*nearFarRatio;
        if (desired_znear<min_near_plane) desired_znear=min_near_plane;

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
    MyClampProjectionMatrixCallback(double nearFarRatio) : _nearFarRatio(nearFarRatio)
    {}

    virtual bool clampProjectionMatrixImplementation(osg::Matrixf& projection, double& znear, double& zfar) const
    {
        return _myclampProjectionMatrix( projection, znear, zfar, _nearFarRatio );
    };
    virtual bool clampProjectionMatrixImplementation(osg::Matrixd& projection, double& znear, double& zfar) const
    {
        return _myclampProjectionMatrix( projection, znear, zfar, _nearFarRatio );
    };

    double _nearFarRatio;
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
    Setup();
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
        m_viewerArgs.Set(view->getName(), view);
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
    double rotationAngle = osg::inDegrees(m_angleBetweenScreensInDegrees/2.0);
    ScreenInfo leftScreen(-rotationAngle);
    ScreenInfo rightScreen(rotationAngle);
    m_screen.clear();
    m_screen.push_back(leftScreen);
    m_screen.push_back(rightScreen);
    m_virtualOrigin = osg::Vec3(0.0, m_screen[0].center.y(), 0);
    m_virtualCenter = osg::Vec3(0.0, m_screen[0].center.y(), m_screen[0].zNear);
    m_viewMatrix.makeLookAt(m_virtualCenter, m_virtualOrigin, osg::Vec3(0,1,0));
    m_projectionMatrix.set(1.0, 0.0, 0.0, 0.0,
                           0.0, 1.0, 0.0, 0.0,
                           0.0, 0.0, 1.0, 0.0,
                           0.0, 0.0, 0.0, 1.0);
    for(int screenIndex = Left; screenIndex < NumOfScreens; screenIndex++)
    {
        CreateViewOffset(screenIndex);
        CreateProjectionOffset(screenIndex);
    }
}

void DualScreenViewer::Update(IArgs *results) 
{
    osgViewer::View* view = static_cast<osgViewer::View*>(m_viewerArgs.Get("main"));
    osgGA::KeySwitchMatrixManipulator* keyManipulator = static_cast<osgGA::KeySwitchMatrixManipulator*>(view->getCameraManipulator());
    if (keyManipulator)
    {
        for (unsigned int i=0; i< keyManipulator->getNumMatrixManipulators(); i++)
        {
            osgGA::CameraManipulator* cameraManipulator = keyManipulator->getMatrixManipulatorWithIndex(i);
            HandleManipulator(cameraManipulator, results);
        }
    }

}

void DualScreenViewer::HandleManipulator(osgGA::CameraManipulator* cameraManipulator, IArgs *results)
{
    if (cameraManipulator->getName() == "Tracker")
    {
        osgGA::TrackerManipulator* trackerManipulator = static_cast<osgGA::TrackerManipulator*>(cameraManipulator);
        trackerManipulator->setTrackingResults(results, m_virtualCenter);
        osg::Quat rotation;
        osg::Vec3d eye;
        trackerManipulator->getTransformation(eye, rotation);
        m_virtualEye = eye;
    }
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


void DualScreenViewer::CreateViewOffset(int screenIndex)
{
    osg::Vec3 pa(m_screen[screenIndex].left, m_screen[screenIndex].bottom, m_screen[screenIndex].zLeft);
    osg::Vec3 pb(m_screen[screenIndex].right, m_screen[screenIndex].bottom, m_screen[screenIndex].zRight);
    osg::Vec3 pc(m_screen[screenIndex].left, m_screen[screenIndex].top, m_screen[screenIndex].zLeft);
    osg::Vec3 vr = pb - pa;
    vr.normalize();
    osg::Vec3 vu = pc - pa;
    vu.normalize();
    osg::Vec3 vn = vr ^ vu;
    vn.normalize();
    m_viewMatrixOffset[screenIndex].set(vr.x(), vr.y(), vr.z(), 0.0,
                                        vu.x(), vu.y(), vu.z(), 0.0,
                                        vn.x(), vn.y(), vn.z(), 0.0,
                                        0.0,    0.0,    0.0,    1.0);
}

void DualScreenViewer::CreateProjectionOffset(int screenIndex)
{
    double left, right, top, bottom, zNear, zFar, zRatio;

    zNear =  m_virtualCenter.z() - m_screen[screenIndex].zNear;
    zFar = m_virtualCenter.z() - m_screen[screenIndex].zFar;
    zRatio = zNear / zFar;
    right = (m_screen[screenIndex].right - m_virtualCenter.x()) * zRatio;
    left = -(m_virtualCenter.x() - m_screen[screenIndex].left)* zRatio;
    top = (m_screen[screenIndex].top  - m_virtualCenter.y() )* zRatio;
    bottom = -(m_virtualCenter.y()  - m_screen[screenIndex].bottom) * zRatio;
    m_projectionMatrixOffset[screenIndex] =  /*HolographicFrustum*/osg::Matrix::frustum(left, right, bottom, top, zNear, zFar);
    double offset = m_screen[screenIndex].m_tvWidth;
    double xOffset = screenIndex == Left ? -offset : offset;
    m_projectionMatrixOffset[screenIndex].postMultTranslate(osg::Vec3(xOffset, 0 , 0));
}

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
        osg::ref_ptr<MyClampProjectionMatrixCallback> callback = new MyClampProjectionMatrixCallback(camera->getNearFarRatio());
        camera->setClampProjectionMatrixCallback( callback );
        view->addSlave(camera.get(), m_projectionMatrixOffset[i], m_viewMatrixOffset[i]);
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
        ds->setScreenDistance(m_screen[0].m_screenDistance); 
        ds->setScreenHeight(m_screen[0].m_screenHeight);
        ds->setScreenWidth(m_screen[0].m_screenWidth);
        ds->setEyeSeparation(0.05f);
    }
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

    // Get near and far from the Projection matrix.
    const double zNear = proj(3,2) / (proj(2,2)-1.0);
    const double zFar = proj(3,2) / (1.0+proj(2,2));

    // Get the sides of the near plane.
    const double nLeft = zNear * (proj(2,0)-1.0) / proj(0,0);
    const double nRight = zNear * (1.0+proj(2,0)) / proj(0,0);
    const double nTop = zNear * (1.0+proj(2,1)) / proj(1,1);
    const double nBottom = zNear * (proj(2,1)-1.0) / proj(1,1);

    // Get the sides of the far plane.
    const double fLeft = zFar * (proj(2,0)-1.0) / proj(0,0);
    const double fRight = zFar * (1.0+proj(2,0)) / proj(0,0);
    const double fTop = zFar * (1.0+proj(2,1)) / proj(1,1);
    const double fBottom = zFar * (proj(2,1)-1.0) / proj(1,1);

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


    // Create parent MatrixTransform to transform the view volume by
    // the inverse ModelView matrix.
    osg::MatrixTransform* mt = new osg::MatrixTransform;
    mt->setMatrix( osg::Matrixd::inverse( mv ) );
    mt->addChild( geode );

    return mt;
}
