#include "osgLib.h"
#ifndef _DUALSCREENVIEWER_H
#define _DUALSCREENVIEWER_H
#include "osgExtension.h"
#include "Args.h"
#include "ScreenInfo.h"
#include "TiltedScreen.h"
#include "MatrixExtension.h"
#include "OsgWidgetDriver.h"
#include "Callback.h"

typedef bool (*ViewCompareFunction)(osgViewer::View* view);


class DualScreenViewer : public osgViewer::CompositeViewer, public Callback
{

public:
    enum TV_SIDE
    {
        Right = 0,
        Left = 1,
        NumOfScreens = 2
    };

    DualScreenViewer(bool initialize = true, bool start = true);
    ~DualScreenViewer();
    bool    IsInitialized;
    bool    IsRunning;

    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };

    inline osg::Vec3& GetVirtualOrigin() { return m_virtualOrigin; }
    inline void SetVirtualOrigin(osg::Vec3& origin) { m_virtualOrigin = origin; }
    inline osg::Vec3& GetVirtualCenter() { return m_virtualCenter; }
    inline void SetVirtualCenter(osg::Vec3& center) { m_virtualCenter = center; }
    inline osg::Vec3& GetVirtualEye() { return m_virtualEye; }
    inline void SetVirtualEye(osg::Vec3& eye) { m_virtualEye = eye; }
    virtual bool Init();
    virtual void Update();
    osgViewer::View* GetMainView()  { return GetView(isMainView); }
    osgViewer::View* GetMapView()  { return GetView(isMapView); }
    static bool isMainView(osgViewer::View* view) { return view->getName() == "main"; }
    static bool isMapView(osgViewer::View* view) { return view->getName() == "map"; }
    void ToggleStereoSettings(osgViewer::View* view);
    osg::MatrixTransform* DualScreenViewer::makeFrustumFromCamera( osgViewer::View* view );
protected:
    virtual bool InitViews();
    virtual bool InitGraphics(osgViewer::View* view);
    osgViewer::View* GetView(ViewCompareFunction fun);
    virtual void SetupView();
    virtual void SetupProjection();
    virtual void CreateGraphicsWindow(osgViewer::View* view);
    virtual void Update(osgViewer::View* view, osg::Vec3 eye);
    virtual void HandleManipulator(osgGA::CameraManipulator* cameraManipulator);

    static void UpdateMap(void* instance);
    ScreenInfo m_display;
    std::vector<TiltedScreen> m_displays;
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