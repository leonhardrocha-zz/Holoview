#include "qtLib.h"
#include "KinectTracker.h"
#include "AppArgs.h"
#include "HoloWindow.h"
#include "MainWindow.h"
#include "DualScreenViewer.h"
#include "HoloviewHelper.h"
#include <QtWidgets/QApplication>

#ifdef _DEBUG
#include "vld.h"
#endif
    



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMap<QString, QSize> customSizeHints = HoloviewHelper::parseCustomSizeHints(argc, argv);

    // main window
    HoloWindow window(customSizeHints);

    OsgViewerGrid *views = new OsgViewerGrid(QString("Views"));
    window.addDockWidget(Qt::LeftDockWidgetArea, views);
    
    KinectTracker tracker;

    int numOfSensors = tracker.GetNumOfSensors();
    for (int i =0; i< numOfSensors; i++)
    {
        TrackerFrame *trackerframe = new TrackerFrame(QString("Tracker "+i));
        trackerframe->SetTracker(&tracker);
        window.addDockWidget(Qt::RightDockWidgetArea, trackerframe);
    }

    //window.AddOsgDockWidget(window.centralWidget());
    //window.AddTrackerDockWidget(static_cast<ITracker*>(&tracker), &args);

    
    DualScreenViewer dualViewer;
    HoloviewHelper::Init(dualViewer);

    // tracker
    window.Init(&dualViewer);
    window.workerThread.start();

    //top level tracker update callback
    dualViewer.Set(HoloviewHelper::ViewUpdateStatic, &dualViewer);
    tracker.AddTrackEventCallback(Callback(HoloviewHelper::TrackerUpdateStatic, &tracker));
    // run all modules
    window.show();
    return app.exec();
}



