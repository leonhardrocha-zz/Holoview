//------------------------------------------------------------------------------
// <copyright file="KinectController.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma once
#endif

#include "Callback.h"
#include "ITracker.h"
#include "IArgs.h"
#include "Attitude.h"
#include "KinectSensor.h"

class KinectController : public KinectSensor
{
public:
    KinectController() : KinectSensor() { };
    ~KinectController() {  Stop(); Release(); };
    virtual bool    Init(IArgs* args=NULL) { return KinectSensor::Init(args); };
    virtual bool    Start(IArgs* args=NULL);
    virtual void    TrackEvent(IArgs* args=NULL);
    bool Release();
    bool Stop();
protected:
    virtual LONG    GetTiltAngle() { return m_tiltAngle; }

/// <summary>
/// Coerce the requested elevation angle to a valid angle
/// </summary>
/// <param name="tiltAngle">The requested angle</param>
/// <returns>The angle after coerced</returns>
    inline LONG CoerceElevationAngle(LONG angle)
    {
        return min(max(NUI_CAMERA_ELEVATION_MINIMUM, angle), NUI_CAMERA_ELEVATION_MAXIMUM);
    }
private:
    ITracker*                   m_parent;
    HWND                        m_hWnd;
    static DWORD WINAPI         TiltTrackingStaticThread(PVOID lpParam);


private:
    LONG m_tiltAngle;
    HANDLE m_hSetTiltAngleEvent;
    HANDLE m_hExitThreadEvent;
    Attitude m_kinectAttitude;
    // Handle to the elevation task thread
    HANDLE m_hElevationTaskThread;
};