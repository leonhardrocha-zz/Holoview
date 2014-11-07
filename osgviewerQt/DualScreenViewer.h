#include "osgLib.h"
#ifndef _DUALSCREENVIEWER_H
#define _DUALSCREENVIEWER_H
#include "osgExtension.h"
#include "Args.h"
#include "ScreenInfo.h"
#include "TiltedScreen.h"
#include "MatrixExtension.h"
#include "OsgWidgetDriver.h"

class DualScreenViewer : public osgViewer::CompositeViewer
{

public:
    enum TV_SIDE
    {
        Right = 0,
        Left = 1,
        NumOfScreens = 2
    };

    DualScreenViewer(bool swapViews = true);
    ~DualScreenViewer();
    virtual bool Init(IArgs* args=NULL);
    virtual void Setup() { Init(); }; //deprecated
    virtual void CreateGraphicsWindow(osgViewer::View* view);
    virtual void Update(IArgs* results);
    virtual void SetupView();
    virtual void SetupProjection();
    virtual void Update(osgViewer::View* view, osg::Vec3 eye);
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    osg::Vec3& GetVirtualOrigin() { return m_virtualOrigin; };
    osg::Vec3& GetVirtualCenter() { return m_virtualCenter; };
    osg::Vec3& GetVirtualEye() { return m_virtualEye; };
    osg::Matrix GetMasterProjectionMatrix() { return m_projectionMatrix; };
    osg::Matrix GetSlaveProjectionMatrix(int side) { return m_projectionOffset[side]; };
    osg::Matrix GetMasterViewMatrix() { return m_viewMatrix; };
    osg::Matrix GetSlaveViewMatrix(int side) { return m_viewOffset[side]; };
    osg::Quat& GetInverseAttitude() { return m_inverseAttitude; };
    void ToggleStereoSettings(osgViewer::View* view);
    osg::MatrixTransform* DualScreenViewer::makeFrustumFromCamera( osgViewer::View* view );
    static void UpdateMap(void* instance, IArgs* args);
    IArgs* GetViewerArgs() { return m_viewerArgs; };
    OsgWidgetDriver* GetDriver() { return &m_driver; };
    osgViewer::View* GetMainView()  { return static_cast<osgViewer::View*>(m_viewerArgs->Get("main")); }
    osgViewer::View* GetMapView()  { return static_cast<osgViewer::View*>(m_viewerArgs->Get("map")); }
protected:
    virtual bool InitViews(IArgs* args);
    IArgs* m_viewerArgs;
    ScreenInfo m_display;
    std::vector<TiltedScreen> m_displays;
    OsgWidgetDriver m_driver;
    virtual void HandleManipulator(osgGA::CameraManipulator* cameraManipulator, IArgs *results=NULL);
    osg::Geode* DrawFrustum(ScreenInfo& info);
    osg::Geode* DrawFrustum(TiltedScreen& info);
    osg::Geometry* GetFrustumGeometry(ScreenInfo& info);
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    osg::Node* m_frustumNode;
    osg::Matrixd m_viewMatrix;
    osg::Matrixd m_projectionMatrix;
    osg::Matrixd m_viewOffset[2];
    osg::Matrixd m_projectionOffset[2];
    osg::Matrixd m_postViewOffset[2];
    osg::Matrixd m_postProjectionOffset[2];
    osg::Quat m_inverseAttitude;
    osg::Vec3 m_virtualEye;
    osg::Vec3 m_virtualCenter;
    osg::Vec3 m_virtualOrigin;
    bool m_swapScreens;
    double m_angleBetweenScreensInDegrees;
};
#endif