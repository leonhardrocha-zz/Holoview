#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _ICALLABLE_H
#define _ICALLABLE_H

#include "IArgs.h"

typedef void (*ICallback)(void* instance);

class ICallable
{
public:
    virtual void Set(ICallback callback, void* instance = NULL)=0;
    virtual void Call()=0;
    virtual void SyncCall()=0;
};


#endif