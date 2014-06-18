#include "stdafx.h"
#include "Callable.h"
#include "HoloWindow.h"
#include "AssetWindow.h"
#include "MainWindow.h"
#include "ITrackerFactory.h"
#include "Factory.h"
#include "WindowsKinectFactory.h"
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
        osgGA::TrackballManipulator* cameraManipulator = static_cast<osgGA::TrackballManipulator*>(myViewer->getCameraManipulator());
        osg::Vec3d eye;
        osg::Vec3d center;
        osg::Vec3d up;
        cameraManipulator->setElevation(osg::inDegrees(90.0));
        cameraManipulator->getTransformation(eye, center, up);
        cameraManipulator->setHomePosition(eye, center, up);
    }

}

void TrackerViewUpdateStatic(void* lpParam, TrackingArgs args=NULL)
{
    KinectTracker* pThis = reinterpret_cast<KinectTracker*>(lpParam);
    TrackingResults* results = pThis->GetTrackingResults();
    osgViewer::Viewer* myViewer = reinterpret_cast<osgViewer::Viewer*>(args->GetArgValue("viewer"));
    if(myViewer)
    {
        osgGA::TrackballManipulator* cameraManipulator = static_cast<osgGA::TrackballManipulator*>(myViewer->getCameraManipulator());
        Pose avatarPose  = results->GetAvatarPose();
        Pose cameraPose  = results->GetCameraPose();
        osg::Quat q(avatarPose.eulerAngles.x, osg::Vec3(1,0,0),
                    avatarPose.eulerAngles.y, osg::Vec3(0,1,0),
                    avatarPose.eulerAngles.z, osg::Vec3(0,0,1));
        /*cameraManipulator->setRotation(q);*/
        osg::Vec3d eye;
        osg::Vec3d center;
        osg::Vec3d up;
        osg::Vec3d deye(avatarPose.translation.x * 100 , avatarPose.translation.y * 100, avatarPose.translation.z* 100); // normalize within kinect range
        osg::Vec3d dcenter(avatarPose.translation.x * 100 , avatarPose.translation.y * 100, 0); // normalize within kinect range
        
        cameraManipulator->home(0.0);
        cameraManipulator->getTransformation(eye, center, up);
        
        osg::Vec3d neweye = eye + deye;
        osg::Vec3d newcenter = eye + dcenter;
        cameraManipulator->setTransformation(neweye, newcenter, up);
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
    TrackerViewInitStatic(&tracker, &args);
    mainWindow.show();
	return app.exec();
}


