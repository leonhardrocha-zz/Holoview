#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"
#include "Singleton.h"
#include "Args.h"

class TrackerBase : public ITracker
{    
public:
    ~TrackerBase() { };
    virtual bool    Init()=0;
    virtual bool    Start()=0;
    virtual void    TrackEvent()=0;
    IArgs* GetArgs() { return &Singleton<Args>::Instance(); }
protected:
private:

};

