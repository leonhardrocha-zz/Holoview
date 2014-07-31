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
            osg::Vec3d center(dualViewer->GetVirtualCenter());
            osg::Vec3 origin(dualViewer->GetVirtualOrigin());
            osg::Vec3d up(0.0, 1.0, 0.0);
            cameraManipulator->setHomePosition(center, origin, up);
            cameraManipulator->home(0.0);
        }
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
            osg::Vec3d kinectBasePosition(0.0, 0.615, 0.1);
            double kinectTiltAngle = osg::inDegrees(20.0);
            const osg::Vec3d kinectEyeOffset(0.0, 0.06, 0.0);
            osg::Vec3d kinectEyePosition = kinectBasePosition + osg::Matrix::rotate(-kinectTiltAngle, osg::Vec3(1.0, 0.0, 0.0)) * kinectEyeOffset;
            osg::Vec3d kinectFrustumOffset( 0.0, avatarPose.translation.z * sin(kinectTiltAngle), avatarPose.translation.z * cos(kinectTiltAngle) );
            osg::Vec3d kinectFrustumTargetPosition = kinectEyePosition + kinectFrustumOffset;
            osg::Vec3d avatarScreenOffset(avatarPose.translation.x, avatarPose.translation.y, avatarPose.translation.z);
            osg::Vec3d new_eye(avatarScreenOffset + osg::Vec3(0.0, kinectFrustumTargetPosition.y(), 0.0));
            dualViewer->UpdateEyeOffset(new_eye);
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMap<QString, QSize> customSizeHints = parseCustomSizeHints(argc, argv);
    HoloWindow mainWindow(customSizeHints);
    KinectTracker tracker;
    tracker.InitDefault();
    DualScreenViewer* dualViewer = static_cast<DualScreenViewer*>(mainWindow.GetViewer());
    ArgsMap args;
    args.AddArg("dualViewer", static_cast<void*>(dualViewer));
    tracker.SetTrackerCallback(TrackerViewUpdateStatic, &tracker, &args);
    mainWindow.AddTrackerDockWidget(&tracker);
    mainWindow.AddOsgDockWidget(mainWindow.centralWidget());
    tracker.Start();
    mainWindow.show();
    TrackerViewInitStatic(&tracker, &args);
    return app.exec();
}



