//------------------------------------------------------------------------------
// <copyright file="TrackerManager.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#pragma once

#include <FaceTrackLib.h>
#include "resource.h"
#include "EggAvatar.h"
#include "KinectFaceTracker.h"
#include "TrackerCallBack.h"
#include "ITracker.h"
#include "IArgs.h"
#include "IPose.h"
#include <vector>


class KinectTracker;

class TrackerManager : public ITracker, public TrackerCallback 
{
    friend class KinectTracker;
public:
    TrackerManager::TrackerManager(ITracker* parent=NULL, IArgs* args=NULL)
        : m_parent(parent)
        , m_lpCmdLine(L"")
        , m_hAccelTable(NULL)
        , m_depthType(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX)
        , m_colorType(NUI_IMAGE_TYPE_COLOR)
        , m_depthRes(NUI_IMAGE_RESOLUTION_320x240)
        , m_colorRes(NUI_IMAGE_RESOLUTION_640x480)
        , m_bNearMode(TRUE)
        , m_bSeatedSkeletonMode(TRUE)
{
}
    ~TrackerManager()    { UninitInstance();    };

    virtual bool                Init();
    virtual bool                Start();
    virtual IArgs*              GetTrackingResults(IArgs* args=NULL);
    virtual void                PaintEvent(void *message, IArgs* args=NULL);
    virtual void                TrackEvent(void *message, IArgs* args=NULL);
    virtual void*               GetCriticalSection() { return static_cast<void*>(&m_CriticalSection); };

    int                         GetNumOfTrackingSensors() { return m_numOfSensors; };
    int                         GetNumOfAvailableSensors() { return m_maxNumOfSensors;};

    void                        InitArgs(int argc, char **argv);
    BOOL                        InitInstanceInHostWindow();

protected:

    int                         m_numOfSensors;
    int                         m_maxNumOfSensors;
    CRITICAL_SECTION            m_CriticalSection;    

    std::vector<KinectFaceTracker*> m_pFaceTrackers;
    std::vector<HANDLE>             m_FaceTrackingThreads;

    ITracker*                   m_parent;
    KinectFaceTracker*          m_pBestTracker;
    PWSTR                       m_lpCmdLine;
    int                         m_nCmdShow;
    void                        ParseCmdString(PWSTR lpCmdLine);
    void                        UninitInstance();
    bool                        IsTracking();
    virtual KinectFaceTracker*  GetBestTracker(IArgs* args=0);
    static int const            MaxLoadStringChars = 100;

    HACCEL                      m_hAccelTable;
    NUI_IMAGE_TYPE              m_depthType;
    NUI_IMAGE_TYPE              m_colorType;
    NUI_IMAGE_RESOLUTION        m_depthRes;
    NUI_IMAGE_RESOLUTION        m_colorRes;
    BOOL                        m_bNearMode;
    BOOL                        m_bSeatedSkeletonMode;
};

class MultiTrackerManager : public TrackerManager
{
protected:
    virtual        BOOL            PaintWindow(KinectFaceTracker *tracker, HDC hdc, HWND hWnd); 

};