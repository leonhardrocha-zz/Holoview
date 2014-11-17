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

TrackerManager::TrackerManager(ITracker* parent, bool initialize, bool start) : m_parent(parent)
{
    if(initialize)
    {
        IsInitialized = Init();
    }
    if (IsInitialized && start)
    {
        IsRunning = Start();
    }
}

TrackerManager::~TrackerManager()
{ 
    Stop();
    for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
    {
        if(*tracker)
        {
            delete *tracker;
        }
    }

}

bool TrackerManager::Start()
{
    for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
    {
        if ((*tracker)->IsRunning)
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

bool TrackerManager::AddTrackEventCallback(const Callback& callback)
{
    for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
    {
        KinectFaceTracker* ftTracker = (*tracker);
        if (ftTracker->IsInitialized && ftTracker->IsRunning)
        {
            TrackerCallbacks.push_back(callback);
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

bool TrackerManager::SetTrackerUpdateCallback(const Callback& callback)
{
    for (std::vector<KinectFaceTracker*>::iterator itr = m_pFaceTrackers.begin(); itr != m_pFaceTrackers.end(); ++itr)
    {
        KinectFaceTracker* pTracker = (*itr);
        if (pTracker->IsInitialized && pTracker->IsRunning)
        {
            pTracker->UpdateCallback = callback;
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

bool TrackerManager::Init()
{
    NuiGetSensorCount(&m_numOfSensors);
    for (int id=0; id < m_numOfSensors; id++)
    {
        KinectFaceTracker* tracker = new KinectFaceTracker(this);
        m_pFaceTrackers.push_back(tracker);
    }          
    return true;
}

void TrackerManager::Stop()
{
    for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
    {
        (*tracker)->Stop();
    }
    IsRunning = false;
}

void TrackerManager::TrackEvent(void* message)
{
    for (std::vector<Callback>::iterator itr = TrackerCallbacks.begin(); itr != TrackerCallbacks.end(); ++itr)
    {
        Callback callback = (*itr);
        if (callback.GetInstance())
        {
            callback.SyncCall();
        }
    }

    if (m_parent)
    {
        m_parent->TrackEvent(message);
    }
};


bool SortFaceTracking (KinectFaceTracker* i,KinectFaceTracker* j) 
{ 
    return (i->GetFaceConfidence()>j->GetFaceConfidence()); 
}

KinectFaceTracker* TrackerManager::GetBestTracker()
{    
    if (!m_pFaceTrackers.empty())
    {
        m_pBestTracker = *(m_pFaceTrackers.begin());
    }
    return m_pBestTracker;
}




