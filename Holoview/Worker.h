#include "qtLib.h"
#include "osgLib.h"
#ifndef WORKER_H
#define WORKER_H

class Worker : public QObject {
    Q_OBJECT
 
public:
    Worker(osgViewer::ViewerBase *viewer) : _viewer(viewer) { };
    ~Worker() {};
 
public slots:
    void Worker::process() 
    {
    // allocate resources using new here
        if (_viewer)
        {
            _viewer->run();
        }

        emit finished();
    }
 
signals:
    void finished();
    void error(QString err);
 
private:
    // add your variables here
    osgViewer::ViewerBase* _viewer;

};

#endif