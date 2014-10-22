#ifndef _OsgView_H_
#define _OsgView_H_

#include "PickHandler.h"
#include <osgViewer/CompositeViewer>

class OsgView
{
public:
// Attributes
public:
    OsgView::OsgView() : width(1024), height(768) {};
    ~OsgView() {};
// Operations
public:
// Overrides
    virtual void Init();
    virtual void ResetView();
    virtual void SetupView();
    virtual void ResetScene();
    virtual void SetupScene();
    virtual void RenderView();
    virtual void RenderScene();
    int width;
    int height;
    float aspect;
protected:
};

#endif