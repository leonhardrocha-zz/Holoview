#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _IIPC_H
#define _IIPC_H
#include "IArgs.h"

class IIPC
{
public:
    virtual bool    Init(IArgs* args=NULL)=0;
    virtual bool    Start(IArgs* args=NULL)=0;
    virtual void    Send(IArgs* args=NULL)=0;
    virtual void*   Receive(IArgs* args=NULL)=0;
};

#endif