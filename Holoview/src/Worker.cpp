#include "qtLib.h"
#include "Worker.h"

Worker::~Worker()
{ 
    if (_viewer) 
    { 
        while (_viewer->areThreadsRunning())
        {
            _viewer->stopThreading();
        }
    }
}

void Worker::process()
{
    // allocate resources using new here
    if (_viewer)
    {
        _viewer->run();
    }

    emit finished();
}
