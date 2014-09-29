#include "osgLib.h"
#ifndef _DUALSCREENVIEWER_H
#define _DUALSCREENVIEWER_H
#include "ObliqueCamera.h"
#include "ViewerArgs.h"
#include "ScreenInfo.h"
#include "TiltedScreen.h"
#include "MatrixExtension.h"

class DualScreenViewer : public osgViewer::CompositeViewer
{

public:
    enum TV_SIDE
    {
        Right = 0,
        Left = 1,
        NumOfScreens = 2
    };

    struct SlaveCallback : public osg::View::Slave::UpdateSlaveCallback
    {
    public:

        virtual void updateSlave(osg::View& view, osg::View::Slave& slave)
        {
            osg::Camera* slaveCamera = slave._camera.get();
            osg::Camera* viewCamera = view.getCamera();
        
            if (!slaveCamera|| !viewCamera) return;

            if (slaveCamera->getReferenceFrame()==osg::Transform::RELATIVE_RF)
            {
                osg::Matrix viewMatrix = viewCamera->getViewMatrix();
                osg::Matrix projMatrix = viewCamera->getProjectionMatrix();
                slaveCamera->setProjectionMatrix(slave._viewOffset * projMatrix * slave._projectionOffset);
                slaveCamera->setViewMatrix(viewMatrix);
            }

            slaveCamera->inheritCullSettings(*viewCamera, slaveCamera->getInheritanceMask());
        };

    };

    DualScreenViewer();
    ~DualScreenViewer();
    virtual void Setup();
    virtual void CreateGraphicsWindow(osgViewer::View* view);
    virtual void Update(IArgs* results);
    virtual void SetupView(osg::Vec3 eye = osg::Vec3(0,0,0));
    virtual void SetupProjection(osg::Vec3 eye = osg::Vec3(0,0,0));
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
    IArgs* GetViewerArgs() { return &m_viewerArgs; };
protected:
    ScreenInfo m_display;
    ViewerArgs m_viewerArgs;
    virtual void HandleManipulator(osgGA::CameraManipulator* cameraManipulator, IArgs *results=NULL);
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    osg::Node* m_frustumNode;
    osg::Matrixd m_viewMatrix;
    osg::Matrixd m_projectionMatrix;
    osg::Matrixd m_viewOffset[2];
    osg::Matrixd m_projectionOffset[2];
    osg::Quat m_inverseAttitude;
    osg::Vec3 m_virtualEye;
    osg::Vec3 m_virtualCenter;
    osg::Vec3 m_virtualOrigin;
    double m_angleBetweenScreensInDegrees;
};
#endif