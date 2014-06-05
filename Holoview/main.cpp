#include "stdafx.h"
#include "Holoview.h"
#include "AssetWindow.h"
#include "MainWindow.h"
#include "ITrackerFactory.h"
#include "Factory.h"
#include "WindowsKinectFactory.h"

#include "HoloviewApp.h"
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


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMap<QString, QSize> customSizeHints = parseCustomSizeHints(argc, argv);
    Holoview mainWindow(customSizeHints);
    ITracker* tracker = mainWindow.GetTracker();
    tracker->Init();
    TrackerManager::MaxNumOfSensors = 1;
    TrackerManager::NumOfSensors = 1;
    mainWindow.AddMultiTrackerDockWidget(tracker);
    tracker->Start();
    mainWindow.show();
	return app.exec();
}

//int main( int argc, char** argv )
//{
//    osg::ArgumentParser arguments(&argc, argv);
//
//#if QT_VERSION >= 0x050000
//    // Qt5 is currently crashing and reporting "Cannot make QOpenGLContext current in a different thread" when the viewer is run multi-threaded, this is regression from Qt4
//    osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::SingleThreaded;
//#else
//    osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
//#endif
//
//    while (arguments.read("--SingleThreaded")) threadingModel = osgViewer::ViewerBase::SingleThreaded;
//    while (arguments.read("--CullDrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
//    while (arguments.read("--DrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::DrawThreadPerContext;
//    while (arguments.read("--CullThreadPerCameraDrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext;
//
//    QApplication app(argc, argv);
//    ViewerWidget* viewWidget;
//
//	try
//    { 
//		viewWidget = new ViewerWidget();
//		viewWidget->setGeometry( 100, 100, 800, 600 );
//		viewWidget->show();
//        /*root = osgDB::readNodeFile("cessna.osg");
//        viewer.setSceneData(root.get()); */
//    }
//    catch(std::bad_alloc)
//    { 
//        std::cout << "a bad_alloc exception just occured"; 
//    }
//    
//    return app.exec();
//}
