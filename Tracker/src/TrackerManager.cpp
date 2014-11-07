#ifdef _MSC_VER
#pragma once
#endif

//------------------------------------------------------------------------------
// <copyright file="TrackerManager.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Defines the entry point for the application.
//
#include "stdafx.h"
#include "TrackerManager.h"
#include "TrackerException.h"

bool TrackerManager::Start(IArgs* args)
{
    for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
    {
        if ((*tracker)->Start(args))
        {
            m_FaceTrackingThreads.push_back((*tracker)->GetThreadId());
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

bool TrackerManager::Init(IArgs* args)
{
    NuiGetSensorCount(&m_numOfSensors);
    int id;
    bool initOk = true;
    InitializeCriticalSection(&m_CriticalSection);
    for (id=0; id < m_numOfSensors; id++)
    {
        KinectFaceTracker* tracker = new KinectFaceTracker(this, id);
        if (tracker->Init(args))
        {
            m_pFaceTrackers.push_back(tracker);
        } else
        {
            initOk = false;
            delete tracker;
        }
    }          
    return initOk;
}

void TrackerManager::UninitInstance()
{
    // Clean up the memory allocated for Face Tracking and rendering.
    for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
    {
        (*tracker)->Stop();
        delete *tracker;
    }

    if (m_hAccelTable)
    {
        DestroyAcceleratorTable(m_hAccelTable);
        m_hAccelTable = NULL;
    }
}

void TrackerManager::TrackEvent(IArgs* args)
{
    EnterCriticalSection(&m_CriticalSection);

    KinectFaceTracker* currentTracker = GetBestTracker();

    if (currentTracker)
    {
        currentTracker->TrackEvent(args);

    }
    if (m_parent)
    {
        m_parent->TrackEvent(args);
    }
    
    ICallback callback = this->GetCallback();
    if(callback)
    {
        if (args)
        {
            Call();
        }
    }
    LeaveCriticalSection(&m_CriticalSection);
};


bool SortFaceTracking (KinectFaceTracker* i,KinectFaceTracker* j) 
{ 
    return (i->GetFaceConfidence()>j->GetFaceConfidence()); 
}

KinectFaceTracker* TrackerManager::GetBestTracker(IArgs* args)
{    
    //TODO
    if (!m_pFaceTrackers.empty())
    {
        m_pBestTracker = *(m_pFaceTrackers.begin());
    }
    return m_pBestTracker;
}




