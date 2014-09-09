//------------------------------------------------------------------------------
// <copyright file="KinectPoseController.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#include "TrackerCallback.h"
#include "ITracker.h"
#include "TrackerArgs.h"
#include "Attitude.h"
#include "KinectSensor.h"
#include <NuiApi.h>

class KinectPoseController : public TrackerCallback, public ITracker
{
private:
    KinectPoseController() {};
    KinectPoseController(KinectPoseController&) {};
public:
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="pNuiSensor">The pointer to Nui sensor device instance</param>
    KinectPoseController(ITracker* parent, KinectSensor* pSensor);

    /// <summary>
    /// Destructor
    /// </summary>
   ~KinectPoseController();
protected:
    virtual bool    Init();
    virtual bool    Start();
    virtual IArgs*  GetTrackingResults(IArgs* args=NULL);
    virtual void    PaintEvent(void *message, IArgs* args=NULL);
    virtual bool    PaintWindow(HDC hdc, HWND hWnd);
    virtual void    TrackEvent(void *message, IArgs* args=NULL);
    virtual void*   GetCriticalSection();
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
    void CleanUp();

private:
    KinectSensor* m_pKinectSensor;
    LONG m_tiltAngle;
    TrackerArgs m_results;
    HANDLE m_hSetTiltAngleEvent;
    HANDLE m_hExitThreadEvent;
    Attitude m_kinectAttitude;
    // Handle to the elevation task thread
    HANDLE m_hElevationTaskThread;
};