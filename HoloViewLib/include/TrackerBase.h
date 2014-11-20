#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"

#ifndef TRACKERBASE_H
#define TRACKERBASE_H


class TrackerBase : ITracker
{
public:
    TrackerBase() : isRunning(false), isInitialized(false) {}

    //commmand pattern and NVI
    bool            Init() { if (!isInitialized) isInitialized = do_init(); return isInitialized; }
    bool            Start() { if (!isRunning) isRunning = do_start(); return isRunning; }
    void            TrackEvent(void* message) { do_trackEvent(message); }
    bool            Stop() { if (isRunning) isRunning = !do_stop(); return isRunning; }
    bool            Release() { if (isInitialized) isInitialized = !do_release(); return isInitialized; }
    bool            IsRunning() { return isRunning; }
    bool            IsInitialized() { return isInitialized; }
protected:
    bool                        isRunning;
    bool                        isInitialized;
private:
    bool                        do_autoInit;
    virtual bool                do_init()=0;
    virtual bool                do_start()=0;
    virtual bool                do_stop()=0;
    virtual bool                do_release()=0;
    virtual void                do_trackEvent(void *message)=0;
};

//template <class T>
//
//template <class T>
//T CreateAutoTracker(T& tracker) { return AutoTracker(tracker); };

#endif



