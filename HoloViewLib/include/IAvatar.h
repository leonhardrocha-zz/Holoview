#ifdef _MEMORY_DEBUG
#include "vld.h"
#endif

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _IAVATAR_H
#define _IAVATAR_H
#include "IArgs.h"
#include "IPose.h"
class IAvatar
{
public:
    //virtual IPose* GetAttitude()=0;
    //virtual IPose* GetPosition()=0;
    virtual void TrackEvent(IFTResult* pResult, IArgs* args=NULL)=0;
    //virtual bool SetRotations(double pitchDegrees, double yawDegrees, double rollDegrees)=0;
    //virtual bool SetRandomRotations()=0;
    //virtual bool SetTranslations(double tX, double tY, double tZ)=0;
    //virtual bool SetScaleAndTranslationToWindow(int height, int width)=0;
    //virtual void SetScale(double scale)=0;
    //virtual void SetTranslationX(double X)=0;
    //virtual void SetTranslationY(double Y)=0;
    //virtual bool DrawImage(void* image)=0;
};
#endif