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
    osgViewer::Viewer* myViewer = reinterpret_cast<osgViewer::Viewer*>(args->GetArgValue("viewer"));
    if(myViewer)
    {
        Pose cameraPose  = results->GetCameraPose();
        osgGA::TrackerManipulator* cameraManipulator = static_cast<osgGA::TrackerManipulator*>(myViewer->getCameraManipulator());
        // viewer: (x,z,y);
        osg::Vec3d eye(0.0, 1.2, 1.5);
        osg::Vec3d kinectBasePosition(0.0, 0.615, 0.1);
        double kinectTiltAngle = osg::inDegrees(20.0);
        const osg::Vec3d kinectEyeOffset(0.0, 0.06, 0.0);
        osg::Vec3d kinectEyePosition = kinectBasePosition + osg::Matrix::rotate(-kinectTiltAngle, osg::Vec3(1.0, 0.0, 0.0)) * kinectEyeOffset;
        osg::Vec3d up(0.0, 1.0, 0.0);
        cameraManipulator->setHomePosition(eye, kinectEyePosition, up);
        cameraManipulator->home(0.0);
    }

}

void TrackerViewUpdateStatic(void* lpParam, TrackingArgs args=NULL)
{
    KinectTracker* pThis = reinterpret_cast<KinectTracker*>(lpParam);
    TrackingResults* results = pThis->GetTrackingResults();
    osgViewer::Viewer* myViewer = reinterpret_cast<osgViewer::Viewer*>(args->GetArgValue("viewer"));
    if(myViewer)
    {
        osgGA::TrackerManipulator* cameraManipulator = static_cast<osgGA::TrackerManipulator*>(myViewer->getCameraManipulator());
        Pose avatarPose  = results->GetAvatarPose();
        osg::Vec3d eye;
        osg::Vec3d center;
        osg::Vec3d up;
        cameraManipulator->home(0.0);

        cameraManipulator->getTransformation(eye, center, up);
        osg::Vec3d rhsTranslation(avatarPose.translation.x, avatarPose.translation.y, avatarPose.translation.z);
        osg::Quat r(avatarPose.eulerAngles.x, osg::Vec3(1,0,0),\
                    avatarPose.eulerAngles.y, osg::Vec3(0,1,0),\
                    0.0, osg::Vec3(0,0,1)/*avatarPose.eulerAngles.z, osg::Vec3(0,0,1)*/);

        osg::Vec3d kinectBasePosition(0.0, 0.615, 0.1);
        double kinectTiltAngle = osg::inDegrees(20.0);
        const osg::Vec3d kinectEyeOffset(0.0, 0.06, 0.0);
        osg::Vec3d kinectEyePosition = kinectBasePosition + osg::Matrix::rotate(-kinectTiltAngle, osg::Vec3(1.0, 0.0, 0.0)) * kinectEyeOffset;
        osg::Vec3d kinectFrustumOffset( 0.0, avatarPose.translation.z * sin(kinectTiltAngle), avatarPose.translation.z * cos(kinectTiltAngle) );
        osg::Vec3d kinectFrustumTargetPosition = kinectEyePosition + kinectFrustumOffset;
        osg::Vec3d avatarScreenOffset(avatarPose.translation.x, avatarPose.translation.y, 0.0);
        osg::Vec3d new_eye = avatarScreenOffset + osg::Vec3(0.0, kinectFrustumTargetPosition.y(), kinectFrustumTargetPosition.z());
        osg::Vec3 new_center(0.0, 1.2, new_eye.z()/2 - 0.75);
        osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
        ds->setScreenDistance(new_eye.length());
        myViewer->setDisplaySettings(ds);

        cameraManipulator->setTransformation(new_eye, new_center, up);

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
    
    void* viewer = static_cast<void*>(mainWindow.GetViewer());
    ArgsMap args;
    args.AddArg("viewer", viewer);
    tracker.SetTrackerCallback(TrackerViewUpdateStatic, &tracker, &args);
    mainWindow.AddMultiTrackerDockWidget(&tracker);
    tracker.Start();
    mainWindow.show();
    TrackerViewInitStatic(&tracker, &args);
	return app.exec();
}



