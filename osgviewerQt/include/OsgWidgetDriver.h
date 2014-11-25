#include "osgLib.h"
#ifndef _OSGWIDGETDRIVER_H
#define _OSGWIDGETDRIVER_H

class OsgWidgetDriver {
public:

    bool init(osgViewer::ViewerBase *viewer) 
    {
        initialized = false;
        compositeViewer = static_cast<osgViewer::CompositeViewer*>(viewer);
        if (compositeViewer)
        {
            compositeViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
            compositeViewer->setReleaseContextAtEndOfFrameHint(false);
            initialized = true;
        }
        
        return initialized;
    }

    bool start() 
    {
        return initialized;
    }

    void stop() 
    {
        compositeViewer->stopThreading();
        compositeViewer->setDone(true);
    }

    void updateFrame() 
    {
        if (initialized)
        {
            compositeViewer->frame();
        }
    }

    void addView(osgViewer::View *view) { compositeViewer->addView(view); }
    bool isInitialized() { return initialized; }
protected:
    bool initialized;
    osgViewer::CompositeViewer *compositeViewer;
};

#endif