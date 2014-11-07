//------------------------------------------------------------------------------
// <copyright file="KinectSensor.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma once
#endif

#include "ITracker.h"
#include "EggAvatar.h"
#include "TrackerConfig.h"
#include <FaceTrackLib.h>
#include <NuiApi.h>

class KinectSensor : public ITracker
{
public:
    KinectSensor();
    ~KinectSensor();
    bool        Init(IArgs* args=NULL);
    bool        Start(IArgs* args=NULL);
    void        TrackEvent(IArgs* args=NULL);
    bool        Stop();
    bool        Release();

    EggAvatar*  GetEggAvatar() { return(&m_eggavatar); };
    HRESULT     GetVideoConfiguration(FT_CAMERA_CONFIG* videoConfig);
    HRESULT     GetDepthConfiguration(FT_CAMERA_CONFIG* depthConfig);
    IFTImage*   GetVideoBuffer(){ return(m_VideoBuffer); };
    IFTImage*   GetDepthBuffer(){ return(m_DepthBuffer); };
    float       GetZoomFactor() { return(m_ZoomFactor); };
    POINT*      GetViewOffSet() { return(&m_ViewOffset); };
    bool        IsInitialized() { return m_bNuiInitialized; };
    HRESULT     GetClosestHint(FT_VECTOR3D* pHint3D);
    virtual HRESULT SetTiltAngle(LONG angle) { return NuiCameraElevationSetAngle(angle); }
    virtual HRESULT GetTiltAngle(LONG* angle) { return NuiCameraElevationGetAngle(angle); }
    static int      NumOfInitilizedSensors;
protected:
    static DWORD WINAPI ProcessThread(PVOID pParam);
    void        GotVideoAlert();
    void        GotDepthAlert();
    void        GotSkeletonAlert();
    HRESULT     InitializeSensor(TrackerConfig& config);
    bool        IsTracked(UINT skeletonId) { return(m_SkeletonTracked[skeletonId]);};
    FT_VECTOR3D NeckPoint(UINT skeletonId) { return(m_NeckPoint[skeletonId]);};
    FT_VECTOR3D HeadPoint(UINT skeletonId) { return(m_HeadPoint[skeletonId]);};

    FT_VECTOR3D m_NeckPoint[NUI_SKELETON_COUNT];
    FT_VECTOR3D m_HeadPoint[NUI_SKELETON_COUNT];
    IFTImage*   m_VideoBuffer;
    IFTImage*   m_DepthBuffer;
    IFTResult*  m_pFTResult;
    float       m_smallestDistance;
    bool        m_SkeletonTracked[NUI_SKELETON_COUNT];
    FLOAT       m_ZoomFactor;   // video frame zoom factor (it is 1.0f if there is no zoom)
    POINT       m_ViewOffset;   // Offset of the view from the top left corner.
    HANDLE      m_hNextDepthFrameEvent;
    HANDLE      m_hNextVideoFrameEvent;
    HANDLE      m_hNextSkeletonEvent;
    HANDLE      m_pDepthStreamHandle;
    HANDLE      m_pVideoStreamHandle;
    HANDLE      m_hThNuiProcess;
    HANDLE      m_hEvNuiProcessStop;
    bool        m_bNuiInitialized; 
    int         m_FramesTotal;
    int         m_SkeletonTotal;
    bool        m_bSensorRunning;
    INuiSensor* m_pSensor;
    EggAvatar     m_eggavatar;
    std::string   m_configArg;
    TrackerConfig m_config;

};
