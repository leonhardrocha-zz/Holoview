#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _IVIEW_H_
#define _IVIEW_H_

class IView
{
public:
// Attributes
public:
// Operations
public:
// Overrides
    virtual void Init()=0;
    virtual void ResetView()=0;
    virtual void SetupView()=0;
    virtual void RenderView()=0;
};

#endif