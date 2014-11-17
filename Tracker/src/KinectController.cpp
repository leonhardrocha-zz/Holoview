//------------------------------------------------------------------------------
// <copyright file="KinectController.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "KinectController.h"
#include "Position.h"
#include "KinectTracker.h"

void KinectController::TrackEvent(void* message)
{
    
#if 0
        double* trackedPose = static_cast<double*>(args->Get("position"));
        if (trackedPose)
        {
            double angle = std::ceil(std::atan(trackedPose[1] / trackedPose[2]));
            double angleInDegrees = angle * 180.0 / 3.14159;
            const double angleThreshold = 2 * 180.0 / 3.14159;
            if (std::abs(m_kinectAttitude.Get(Attitude::Pitch) - angleInDegrees) > angleThreshold)
            {
                LONG trackedPitch = CoerceElevationAngle((LONG)angleInDegrees);
                m_kinectAttitude.Set(Attitude::Pitch, (double)trackedPitch);
                args->Set("KinectAttitude", static_cast<void*>(&m_kinectAttitude));
                // Exit the elevation task thread
                SetEvent(m_hSetTiltAngleEvent);
            }
        }
#endif

}

/// <summary>
/// Start a new thread to run the elevation task
/// </summary>
bool KinectController::Start()
{
    // Create the events for elevation task thread
    m_hSetTiltAngleEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    m_hExitThreadEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    m_hElevationTaskThread = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)TiltTrackingStaticThread,
        (LPVOID)this,
        0,
        NULL);

    return KinectSensor::Start();
}

/// <summary>
/// The thread procedure in which the elevation task runs
/// </summary>
/// <param name="pThis">The pointer to KinectController instance</param>
DWORD WINAPI KinectController::TiltTrackingStaticThread(PVOID lpParam)
{
    const int numEvents = 2;
    KinectController* pThis = static_cast<KinectController*>(lpParam);
    HANDLE events[numEvents] = {pThis->m_hSetTiltAngleEvent, pThis->m_hExitThreadEvent};

    while(pThis->IsRunning)
    {
        // Check if we have a setting tilt angle event or an exiting thread event
        DWORD dwEvent = WaitForMultipleObjects(numEvents, events, FALSE, INFINITE);

        if (WAIT_OBJECT_0 == dwEvent)
        {
            Sleep(100);
            //TODO
        }
        else if (WAIT_OBJECT_0 + 1 == dwEvent)
        {
            // Close the handles and exit the thread
            CloseHandle(pThis->m_hSetTiltAngleEvent);
            CloseHandle(pThis->m_hExitThreadEvent);

            break;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

/// <summary>
/// Release all the resources
/// </summary>
bool KinectController::Release()
{
    return KinectSensor::Release();
}
bool KinectController::Stop()
{
    // Exit the elevation task thread
    SetEvent(m_hExitThreadEvent);

    // Wait for the elevation task thread
    WaitForSingleObject(m_hElevationTaskThread, 5000/*INFINITE*/);

    if (m_hElevationTaskThread)
    {
        CloseHandle(m_hElevationTaskThread);
        return true;
    }

    return false;
}