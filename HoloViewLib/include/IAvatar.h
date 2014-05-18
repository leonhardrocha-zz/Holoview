#ifndef _IAVATAR_H
#define _IAVATAR_H
#include "TrackingResults.h"
class IAvatar
{
public:
	virtual AvatarPose* GetPose()=0;
	virtual bool SetRotations(const float pitchDegrees, const float yawDegrees, const float rollDegrees)=0;
    virtual bool SetRandomRotations()=0;
    virtual bool SetTranslations(const float tX, const float tY, const float tZ)=0;
    virtual bool SetScaleAndTranslationToWindow(int height, int width)=0;
    virtual void SetScale(float scale)=0;
    virtual void SetTranslationX(float X)=0;
    virtual void SetTranslationY(float Y)=0;
	virtual bool DrawImage(void* image)=0;
};
#endif