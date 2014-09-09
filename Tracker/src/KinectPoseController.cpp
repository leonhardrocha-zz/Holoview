//------------------------------------------------------------------------------
// <copyright file="KinectPoseController.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "KinectPoseController.h"
#include "Position.h"



KinectPoseController::KinectPoseController(ITracker* parent, KinectSensor* pSensor)
    : m_parent(parent)
    , m_pKinectSensor(pSensor)
    , m_tiltAngle(LONG_MAX)
    , m_hElevationTaskThread(nullptr)
{
}

KinectPoseController::~KinectPoseController()
{
    CleanUp();
}


IArgs* KinectPoseController::GetTrackingResults(IArgs* args)
{
    if (m_parent)
    {
        IArgs* results = m_parent->GetTrackingResults(args);
        IPose* trackedPose = static_cast<IPose*>(results->Get("position"));
        if (trackedPose)
        {
            double angle = std::ceil(std::atan(trackedPose->Get(Position::YAxis) / trackedPose->Get(Position::ZAxis)));
            double angleInDegrees = angle * 180.0 / 3.14159;
            const double angleThreshold = 2 * 180.0 / 3.14159;
            if (std::abs(m_kinectAttitude.Get(Attitude::Pitch) - angleInDegrees) > angleThreshold)
            {
                LONG trackedPitch = CoerceElevationAngle((LONG)angleInDegrees);
                m_kinectAttitude.Set(Attitude::Pitch, (double)trackedPitch);
                results->Set("KinectAttitude", static_cast<void*>(&m_kinectAttitude));
            }
        }
        return results;
    }
    return NULL;
}


bool KinectPoseController::Init()
{
    return m_pKinectSensor->IsInitialized();
}

void* KinectPoseController::GetCriticalSection()
{
    if (m_parent)
    {
        return m_parent->GetCriticalSection();
    }
    return NULL;
}

void KinectPoseController::PaintEvent(void *message, IArgs* args)
{

}

bool KinectPoseController::PaintWindow(HDC hdc, HWND hWnd)
{
    return false;
}

void KinectPoseController::TrackEvent(void *message, IArgs* args)
{
    if(m_parent)
    {
        m_parent->TrackEvent(message);
    }
}

/// <summary>
/// Start a new thread to run the elevation task
/// </summary>
bool KinectPoseController::Start()
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

    return true;
}

/// <summary>
/// The thread procedure in which the elevation task runs
/// </summary>
/// <param name="pThis">The pointer to KinectPoseController instance</param>
DWORD WINAPI KinectPoseController::TiltTrackingStaticThread(PVOID lpParam)
{
    const int numEvents = 2;
    KinectPoseController* pThis = static_cast<KinectPoseController*>(lpParam);
    HANDLE events[numEvents] = {pThis->m_hSetTiltAngleEvent, pThis->m_hExitThreadEvent};

    while(true)
    {
        // Check if we have a setting tilt angle event or an exiting thread event
        DWORD dwEvent = WaitForMultipleObjects(numEvents, events, FALSE, INFINITE);

        if (WAIT_OBJECT_0 == dwEvent)
        {
            IArgs* results = pThis->GetTrackingResults();
            if (results)
            {
                pThis->TrackEvent(results);
            }
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
void KinectPoseController::CleanUp()
{
    // Exit the elevation task thread
    SetEvent(m_hExitThreadEvent);

    // Wait for the elevation task thread
    WaitForSingleObject(m_hElevationTaskThread, INFINITE);

    if (m_hElevationTaskThread)
    {
        CloseHandle(m_hElevationTaskThread);
    }
}
