#include "DualScreenViewer.h"
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
    // Create View 0 -- Just the loaded model.
    {
        osgViewer::View* view = new osgViewer::View;
        osg::DisplaySettings* ds = new osg::DisplaySettings();
        view->setDisplaySettings(ds);
        CreateGraphicsWindow(view);
        ToggleStereoSettings(view);
        ViewUpdateHandler* viewUpdateHandler = new  ViewUpdateHandler();
        viewUpdateHandler->SetCallback(DualScreenViewer::UpdateMap, this);
        view->addEventHandler( viewUpdateHandler );
        addView( view );
    }

    // Create view 1 -- Contains the loaded model, as well as a wireframe frustum derived from View 0's Camera.
    {
        osgViewer::View* view = new osgViewer::View;
        osg::DisplaySettings* ds = new osg::DisplaySettings();
        view->setDisplaySettings(ds);
        view->setUpViewInWindow( 10, 510, 640, 480 );
        view->setCameraManipulator( new osgGA::TrackballManipulator );
        addView( view );
    }

    
    m_traits = new osg::GraphicsContext::Traits;

#ifndef WIN32
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
#endif

    setKeyEventSetsDone(0);
    osgViewer::View* view = getView(Main);
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
    ds->setScreenDistance(m_eyeOffset.length());

    m_projectionMatrix.set(1.0, 0.0, 0.0, 0.0,
                           0.0, 1.0, 0.0, 0.0,
                           0.0, 0.0, 1.0, 0.0,
                           0.0, 0.0, 0.0, 1.0);
    for(int screenIndex = Left; screenIndex < NumOfScreens; screenIndex++)
    {
        CreateViewOffset(screenIndex);
        CreateProjectionOffset(screenIndex);
    }
    /*osg::Camera* camera = getView(0)->getCamera();
    osg::ref_ptr<MyClampProjectionMatrixCallback> callback = new MyClampProjectionMatrixCallback(camera->getNearFarRatio());
    camera->setClampProjectionMatrixCallback( callback );*/
    //osg::CullSettings::CullingMode mode = camera->getCullingMode();
    
    /*mode &= ~osg::CullSettings::NEAR_PLANE_CULLING;
    mode &= ~osg::CullSettings::FAR_PLANE_CULLING;*/
    /*mode |= osg::CullSettings::VIEW_FRUSTUM_CULLING;
    mode |= osg::CullSettings::VIEW_FRUSTUM_CULLING;
    mode |= osg::CullSettings::NO_CULLING;*/
    /*camera->setCullingMode(mode);*/
    //camera->setComputeNearFarMode();
}

DualScreenViewer::~DualScreenViewer() 
{
}

void DualScreenViewer::Update(IArgs *results) 
{
    const osg::Vec3 kinectBasePosition(0.0, 0.615, 0.1);
    const double kinectTiltAngle = osg::inDegrees(20.0);
    IPose *position = static_cast<IPose*>(results->Get("position"));
    osg::Vec3 avatarPosition(position->Get(0), position->Get(1), position->Get(2));
    const osg::Vec3 kinectEyeOffset(0.0, 0.06, 0.0);
    const osg::Vec3 kinectEyePosition = kinectBasePosition + osg::Matrix::rotate(-kinectTiltAngle, osg::Vec3(1.0, 0.0, 0.0)) * kinectEyeOffset;
    const osg::Vec3 kinectFrustumOffset( 0.0, avatarPosition.y() * sin(kinectTiltAngle), avatarPosition.z() * cos(kinectTiltAngle) );
    const osg::Vec3 kinectFrustumTargetPosition = kinectEyePosition + kinectFrustumOffset;
    osg::Vec3 new_eye(avatarPosition.x(), avatarPosition.y() + kinectFrustumTargetPosition.y(), avatarPosition.z());
    {
        osgViewer::View* view = getView(Main);
        osg::Camera* camera = view->getCamera();
        camera->setViewMatrixAsLookAt(new_eye, m_virtualCenter, osg::Vec3(0,1,0));
    }
}

void DualScreenViewer::UpdateMap(void* instance, IArgs* args)
{
    // Update the wireframe frustum
    DualScreenViewer* pThis = static_cast<DualScreenViewer*>(instance);

    osgViewer::View* mainView = pThis->getView(Main);
    osg::Node* scene = mainView->getSceneData();
    osg::Group* root = scene->getParent(0);
    if (root) 
    {
        if (root->getNumChildren() > 1)
        {
            osgViewer::View* mapView = pThis->getView(Map);
            osg::Node* map = root->getChild(1);
            root->replaceChild(map, pThis->makeFrustumFromCamera( mapView ));
        }
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
    double xOffset = screenIndex == Left ? -m_tvWidth : m_tvWidth;
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
        //osg::ref_ptr<MyClampProjectionMatrixCallback> callback = new MyClampProjectionMatrixCallback(camera->getNearFarRatio());
        //camera->setClampProjectionMatrixCallback( callback );
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
        ds->setScreenDistance(m_screenDistance); 
        ds->setScreenHeight(m_screenHeight);
        ds->setScreenWidth(m_screenWidth);
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
