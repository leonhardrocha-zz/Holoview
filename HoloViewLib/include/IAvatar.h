#ifndef _IAVATAR_H
#define _IAVATAR_H
#include "IArgs.h"
#include "IPose.h"
class IAvatar
{
public:
    virtual IPose* GetAttitude()=0;
    virtual IPose* GetPosition()=0;
    virtual bool SetRotations(const double pitchDegrees, const double yawDegrees, const double rollDegrees)=0;
    virtual bool SetRandomRotations()=0;
    virtual bool SetTranslations(const double tX, const double tY, const double tZ)=0;
    virtual bool SetScaleAndTranslationToWindow(int height, int width)=0;
    virtual void SetScale(double scale)=0;
    virtual void SetTranslationX(double X)=0;
    virtual void SetTranslationY(double Y)=0;
	virtual bool DrawImage(void* image)=0;
};
#endif