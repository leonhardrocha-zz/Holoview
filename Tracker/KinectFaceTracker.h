#include "vld.h"

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _KINECTFACETRACKER_H
#define _KINECTFACETRACKER_H

#pragma warning(disable:4786)
#include <FaceTrackLib.h>
#include "ZmqIPC.h"
#include "ITracker.h"
#include "IArgs.h"
#include "TrackerConfig.h"
#include "Callback.h"
#include "KinectSensor.h"
#include "Callback.h"
#include <vector>
#include <queue>
#include <map>
#include <string>

class TrackerManager;

class KinectFaceTracker : ITracker
{
public:

    friend class TrackerManager;

    KinectFaceTracker(ITracker* parent=NULL, bool initialize = true, bool start = true);
    ~KinectFaceTracker();
    virtual bool                Init();
    virtual bool                Start();
    virtual bool                Release();
    virtual bool                Stop();
    virtual void                TrackEvent(void* message);
    HRESULT                     GetTrackerResult();
    void                        CheckCameraInput();
    int                         GetId()             { return(m_id);};
    IFTResult*                  GetResult()         { return(m_pFTResult);};
    IFTImage*                   GetColorImage()     { return(m_colorImage);};
    IFTFaceTracker*             GetFaceTracker();
    KinectSensor*               GetSensor();
    double                      GetFaceConfidence() { return m_faceConfidence; };
    inline void                 GetFaceRotation(double& pitch, double& yaw, double& row) { pitch = rotationXYZ[0]; yaw = rotationXYZ[1]; row = rotationXYZ[2];};
    inline void                 GetFaceTranslation(double& x, double& y, double& z) { x = translationXYZ[0]; y = translationXYZ[1]; z = translationXYZ[2];};
    HANDLE                      GetThreadId()       { return m_hFaceTrackingThread; };
    bool                        GetFaceModel(IFTImage* m_colorImage);
    bool                        LastTrackSucceeded;
    bool                        IsRunning;
    bool                        IsInitialized;
    std::vector<Callback>       TrackerCallbacks;
    Callback                    UpdateCallback;
protected:
    static DWORD WINAPI         FaceTrackingStaticThread(PVOID lpParam);
    HRESULT                     m_trackingStatus;
    RECT                        m_Roi;
//    CRITICAL_SECTION            m_CriticalSection;
//    CRITICAL_SECTION*           m_pCriticalSection;
    ITracker*                   m_parent;
    int                         m_id;
    HANDLE                      m_hFaceTrackingThread;
    IFTFaceTracker*             m_pFaceTracker;
    IFTResult*                  m_pFTResult;
    IFTImage*                   m_colorImage;
    IFTImage*                   m_depthImage;
    KinectSensor*               m_pKinectSensor;
    POINT                       m_viewOffset;

    double                      rotationXYZ[3];
    double                      translationXYZ[3];
    double                      m_faceConfidence;
    TrackerConfig               m_config;
    DWORD                       WINAPI FaceTrackingThread();

    //ZmqIPC                      m_messageQueue;
};

#endif