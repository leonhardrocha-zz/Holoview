#include "qtLib.h"
#include "HoloWindow.h"

HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
    : MainWindow(customSizeHints, parent, flags)
{
}

HoloWindow::~HoloWindow()
{
    /*if(!workerThread.isFinished())
    {
        workerThread.terminate();
    }*/
    if (worker)
    {
        delete worker;
        worker = NULL;
    }
}

void HoloWindow::Init(osgViewer::ViewerBase *viewer)
{
    worker = new Worker(viewer);
    worker->moveToThread(&workerThread);
    connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(&workerThread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), &workerThread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(&workerThread, SIGNAL(finished()), &workerThread, SLOT(deleteLater()));
    workerThread.start();
}

void HoloWindow::addDockWidget(Qt::DockWidgetArea area, QWidget *dockwidget)
{
    MyDock *dock = new MyDock(dockwidget, this);
    dock->setCustomSizeHint(m_customSizeHints.value(dockwidget->objectName()));
    dock->setFloating(false);
    dockWidgetMenu->addMenu(dock->menu);
    MainWindow::addDockWidget(area, dock);
}

//bool MainWindow::AddTrackerDockWidget(ITracker *tracker, IArgs *args)
//{
    //KinectTracker* kinectTracker = dynamic_cast<KinectTracker*>(tracker);
    //if(!kinectTracker) return false;

    //QString name = QString::fromLatin1("Tracker");
    //int numOfSensors = kinectTracker->GetNumOfTrackingSensors();
    //for (int i =0; i< numOfSensors; i++)
    //{
    //    TrackerFrame *frame = new TrackerFrame(this, name);
    //    args->Set(frame->TrackerHandlerArg, tracker);
    //    frame->setFrameStyle(QFrame::Box | QFrame::Sunken);
    //    frame->Init(args);
    //}
//    MyDock *trackerDock = new MyDock(name + i, this, Qt::WindowFlags(0), frame);
//    trackerDock->setCustomSizeHint(m_customSizeHints.value(name + i));
//    addDockWidget(Qt::LeftDockWidgetArea, trackerDock);
//    dockWidgetMenu->addMenu(trackerDock->menu); 
//    
//    return true;
//}

void HoloWindow::handleResults()
{
}
//
//bool HoloWindow::eventFilter(QObject *o, QEvent *e)
//{
//    if (o == centralWidget() && e->type() == QEvent::Paint) 
//    {
//        o->event(e);
//    }
//    return QMainWindow::eventFilter(o, e);
//}