//------------------------------------------------------------------------------
// <copyright file="TrackerManager.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#endif


#include <FaceTrackLib.h>
#include "resource.h"
#include "EggAvatar.h"
#include "KinectFaceTracker.h"
#include "ICallable.h"
#include "CallBack.h"
#include "ITracker.h"
#include "IArgs.h"
#include "IPose.h"
#include <vector>
#include "AppArgs.h"

class KinectTracker;

class TrackerManager : public ITracker
{
    friend class KinectTracker;
public:
    TrackerManager::TrackerManager(ITracker* parent=NULL, bool initialize = true, bool start = true);
    ~TrackerManager();

    virtual bool                Init();
    virtual bool                Start();
    virtual void                Stop();
    virtual void                TrackEvent(void* message);
    int                         GetNumOfSensors() { return m_numOfSensors; };
    bool                        IsRunning;
    bool                        IsInitialized;
    bool                        AddTrackEventCallback(const Callback& callback);
    bool                        SetTrackerUpdateCallback(const Callback& callback);
protected:

    int                         m_numOfSensors;
    std::vector<KinectFaceTracker*> m_pFaceTrackers;
    std::vector<HANDLE>         m_FaceTrackingThreads;
    std::vector<Callback>       TrackerCallbacks;
    ITracker*                   m_parent;
    KinectFaceTracker*          m_pBestTracker;
    virtual KinectFaceTracker*  GetBestTracker();


};