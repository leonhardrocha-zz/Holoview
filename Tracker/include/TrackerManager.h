//------------------------------------------------------------------------------
// <copyright file="TrackerManager.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#endif

#include <FaceTrackLib.h>
#include "EggAvatar.h"
#include "KinectFaceTracker.h"
#include "CallBack.h"
#include "TrackerBase.h"
#include <vector>
#include "AppArgs.h"

class KinectTracker;

class TrackerManager : public TrackerBase
{
    friend class KinectTracker;
public:
    TrackerManager() : TrackerBase() { if(Init()) Start(); }
    ~TrackerManager() { if (Stop()) Release(); }

    int                         GetNumOfSensors() { return m_numOfSensors; };
    bool                        AddTrackEventCallback(const Callback& callback);
    bool                        SetTrackerUpdateCallback(const Callback& callback);
    virtual KinectFaceTracker*  GetBestTracker();
protected:

    int                         m_numOfSensors;
    std::vector<KinectFaceTracker*> m_pFaceTrackers;
    std::vector<Callback>       TrackerCallbacks;
    KinectFaceTracker*          m_pBestTracker;

private:
    virtual bool                do_init();
    virtual bool                do_start();
    virtual bool                do_stop();
    virtual bool                do_release();
    virtual void                do_trackEvent(void* message);

};