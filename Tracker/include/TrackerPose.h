#include "vld.h"
#ifndef _TRACKERPOSE_H
#define _TRACKERPOSE_H
#include "IPose.h"
#include "Position.h"
#include "Attitude.h"

class TrackerPose : public Position, public Attitude
{
public:
    virtual void SetPosition(double* value) { for (int i = XAxis; i < NumOfAxis; i++) { position[i] = value[i];} };
    virtual void SetAttitude(double* value) { for (int i = Pitch; i < NumOfDegrees; i++) { eulerAngle[i] = value[i];} };
    virtual double* GetPosition() { return &position[0]; };
    virtual double* GetAttitude() { return &eulerAngle[0]; };
};

#endif