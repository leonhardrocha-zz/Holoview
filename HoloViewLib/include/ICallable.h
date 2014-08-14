#include "vld.h"
#ifndef _ICALLABLE_H
#define _ICALLABLE_H

#include "IArgs.h"

typedef void (*ICallback)(void* instance, IArgs* args);

class ICallable
{
public:
    virtual void SetCallback(ICallback callback, void* instance = NULL, IArgs* args=NULL)=0;
    virtual void Call()=0;
};


#endif