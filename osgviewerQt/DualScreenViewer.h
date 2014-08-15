#include "osgLib.h"
#ifndef _DUALSCREENVIEWER_H
#define _DUALSCREENVIEWER_H
#include "ViewerArgs.h"
#include "ScreenInfo.h"

class DualScreenViewer : public osgViewer::CompositeViewer
{

public:

    enum TV_SIDE
    {
        Left = 0,
        Right = 1,
        NumOfScreens = 2
    };

    DualScreenViewer();
    ~DualScreenViewer();
    virtual void Setup();
    virtual void CreateGraphicsWindow(osgViewer::View* view);
    virtual void Update(IArgs* results);
    virtual void CreateViewOffset(int screen);
    virtual void CreateProjectionOffset(int screen);
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    osg::Vec3& GetVirtualOrigin() { return m_virtualOrigin; };
    osg::Vec3& GetVirtualCenter() { return m_virtualCenter; };
    osg::Vec3& GetVirtualEye() { return m_virtualEye; };
    osg::Matrix GetMasterProjectionMatrix() { return m_projectionMatrix; };
    osg::Matrix GetSlaveProjectionMatrix(int side) { return m_projectionMatrixOffset[side]; };
    osg::Matrix GetMasterViewMatrix() { return m_viewMatrix; };
    osg::Matrix GetSlaveViewMatrix(int side) { return m_viewMatrixOffset[side]; };
    osg::Quat& GetInverseAttitude() { return m_inverseAttitude; };
    void ToggleStereoSettings(osgViewer::View* view);
    osg::MatrixTransform* DualScreenViewer::makeFrustumFromCamera( osgViewer::View* view );
    static void UpdateMap(void* instance, IArgs* args);
    IArgs* GetViewerArgs() { return &m_viewerArgs; };
protected:

    ViewerArgs  m_viewerArgs;
    virtual void HandleManipulator(osgGA::CameraManipulator* cameraManipulator, IArgs *results=NULL);
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    osg::Node* m_frustumNode;
    osg::Matrix m_viewMatrix;
    osg::Matrix m_projectionMatrix;
    osg::Matrix m_viewMatrixOffset[2];
    osg::Matrix m_projectionMatrixOffset[2];
    std::vector<ScreenInfo> m_screen;
    osg::Quat m_inverseAttitude;
    osg::Vec3 m_virtualEye;
    osg::Vec3 m_virtualCenter;
    osg::Vec3 m_virtualOrigin;
    double m_angleBetweenScreensInDegrees;

};
#endif