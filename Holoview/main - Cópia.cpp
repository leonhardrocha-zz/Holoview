#include "stdafx.h"
#include "Callable.h"
#include "HoloWindow.h"
#include "AssetWindow.h"
#include "MainWindow.h"
#include "ITrackerFactory.h"
#include "MultiViewerWidget.h"
#include <QtWidgets/QApplication>

#ifdef _DEBUG
#include "vld.h"
#endif
	
void usage()
{
    qWarning() << "Usage: mainwindow [-SizeHint<color> <width>x<height>] ...";
    exit(1);
}

QMap<QString, QSize> parseCustomSizeHints(int argc, char **argv)
{
    QMap<QString, QSize> result;

    for (int i = 1; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);

        if (arg.startsWith(QLatin1String("-SizeHint"))) {
            QString name = arg.mid(9);
            if (name.isEmpty())
                usage();
            if (++i == argc)
                usage();
            QString sizeStr = QString::fromLocal8Bit(argv[i]);
            int idx = sizeStr.indexOf(QLatin1Char('x'));
            if (idx == -1)
                usage();
            bool ok;
            int w = sizeStr.left(idx).toInt(&ok);
            if (!ok)
                usage();
            int h = sizeStr.mid(idx + 1).toInt(&ok);
            if (!ok)
                usage();
            result[name] = QSize(w, h);
        }
    }

    return result;
}

struct UpdateDualSlaveCallback : public osg::View::Slave::UpdateSlaveCallback
{
public:
    UpdateDualSlaveCallback()
    {
    };

    virtual void updateSlave(osg::View& view, osg::View::Slave& slave)
    {
        osg::Camera* slaveCamera = slave._camera.get();
        osg::Camera* viewCamera = view.getCamera();
        DualScreenViewer* dualView = dynamic_cast<DualScreenViewer*>(&view);
        if (!slaveCamera|| !viewCamera || !dualView) return;

        if (slaveCamera->getReferenceFrame()==osg::Transform::RELATIVE_RF)
        {
            slaveCamera->setProjectionMatrix(slave._projectionOffset * viewCamera->getProjectionMatrix());
            slaveCamera->setViewMatrix(slave._viewOffset * view.getCamera()->getViewMatrix());
        }

        slaveCamera->inheritCullSettings(*viewCamera, slaveCamera->getInheritanceMask());
    };

    osg::Matrix m_InitialProjectionOffset;
};



void TrackerViewInitStatic(void* lpParam, TrackingArgs args=NULL)
{
    KinectTracker* pThis = reinterpret_cast<KinectTracker*>(lpParam);
    TrackingResults* results = pThis->GetTrackingResults();
    DualScreenViewer* dualViewer = reinterpret_cast<DualScreenViewer*>(args->GetArgValue("dualViewer"));
    if(dualViewer)
    {
        Pose cameraPose  = results->GetCameraPose();
        osgGA::KeySwitchMatrixManipulator* keyManipulator = static_cast<osgGA::KeySwitchMatrixManipulator*>(dualViewer->getCameraManipulator());
        unsigned int numManipulators = keyManipulator->getNumMatrixManipulators();
        for(unsigned int i =0; i< numManipulators; i++) 
        {
            osgGA::CameraManipulator* cameraManipulator = keyManipulator->getMatrixManipulatorWithIndex(i);
            osg::Vec3d eye(0.0, 1.2, 1.5);
            osg::Vec3 center(0.0, 1.2, 0.0);
            osg::Vec3d up(0.0, 1.0, 0.0);
            cameraManipulator->setHomePosition(eye, center, up);
            cameraManipulator->home(0.0);
        }
        osg::View::Slave& leftSlave = dualViewer->getSlave(0);
        osg::View::Slave& rightSlave = dualViewer->getSlave(1);
        leftSlave._updateSlaveCallback = new UpdateDualSlaveCallback();
        rightSlave._updateSlaveCallback = new UpdateDualSlaveCallback();
    }
    
}

void TrackerViewUpdateStatic(void* lpParam, TrackingArgs args=NULL)
{
    KinectTracker* pThis = reinterpret_cast<KinectTracker*>(lpParam);
    TrackingResults* results = pThis->GetTrackingResults();
    DualScreenViewer* dualViewer = reinterpret_cast<DualScreenViewer*>(args->GetArgValue("dualViewer"));
    if(dualViewer)
    {
        osgGA::KeySwitchMatrixManipulator* keyManipulator = static_cast<osgGA::KeySwitchMatrixManipulator*>(dualViewer->getCameraManipulator());
        osgGA::CameraManipulator* cameraManipulator = keyManipulator->getCurrentMatrixManipulator();
        const std::string name = cameraManipulator->getName();
        if (name == "Tracker") 
        {
            osgGA::TrackerManipulator* trackerManipulator = static_cast<osgGA::TrackerManipulator*>(cameraManipulator);
            Pose avatarPose  = results->GetAvatarPose();
            osg::Vec3d eye;
            osg::Vec3d center;
            osg::Vec3d up;
            trackerManipulator->home(0.0);
            trackerManipulator->getTransformation(eye, center, up);

            osg::Vec3d kinectBasePosition(0.0, 0.615, 0.1);
            double kinectTiltAngle = osg::inDegrees(20.0);
            const osg::Vec3d kinectEyeOffset(0.0, 0.06, 0.0);
            osg::Vec3d kinectEyePosition = kinectBasePosition + osg::Matrix::rotate(-kinectTiltAngle, osg::Vec3(1.0, 0.0, 0.0)) * kinectEyeOffset;
            osg::Vec3d kinectFrustumOffset( 0.0, avatarPose.translation.z * sin(kinectTiltAngle), avatarPose.translation.z * cos(kinectTiltAngle) );
            osg::Vec3d kinectFrustumTargetPosition = kinectEyePosition + kinectFrustumOffset;
            osg::Vec3d avatarScreenOffset(avatarPose.translation.x, avatarPose.translation.y, 0.0);
            osg::Vec3d new_eye = avatarScreenOffset + osg::Vec3(0.0, kinectFrustumTargetPosition.y(), 1.5);
            osg::Vec3d screenOffset = avatarScreenOffset;
            osg::Vec3d new_up(0.0, 1.0, 0.0);
            dualViewer->m_EyeProjectionOffset = new_eye;
            dualViewer->CalculateProjectionMatrixOffset(new_eye);
            osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
            ds->setScreenDistance(new_eye.length());
            dualViewer->setDisplaySettings(ds);
            trackerManipulator->setTransformation(new_eye, center, new_up);
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMap<QString, QSize> customSizeHints = parseCustomSizeHints(argc, argv);
    HoloWindow mainWindow(customSizeHints);
    KinectTracker tracker;
    TrackerManager::MaxNumOfSensors = 1;
    TrackerManager::NumOfSensors = 1;
    tracker.Init();
    DualScreenViewer* dualViewer = static_cast<DualScreenViewer*>(mainWindow.GetViewer());
    ArgsMap args;
    args.AddArg("dualViewer", static_cast<void*>(dualViewer));
    tracker.SetTrackerCallback(TrackerViewUpdateStatic, &tracker, &args);
    mainWindow.AddMultiTrackerDockWidget(&tracker);
    tracker.Start();
    mainWindow.show();
    TrackerViewInitStatic(&tracker, &args);
    mainWindow.GetViewer()->run();
    return app.exec();
}



