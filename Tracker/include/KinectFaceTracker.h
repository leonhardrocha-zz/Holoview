//------------------------------------------------------------------------------
// <copyright file="FTHelper.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#pragma warning(disable:4786)
#include <FaceTrackLib.h>

#include "ITracker.h"
#include "IArgs.h"
#include "TrackerArgs.h"
#include "TrackerCallback.h"
#include "TrackerConfig.h"
#include <vector>
#include <queue>
#include <map>

class KinectFaceTracker : public TrackerCallback, ITracker
{
public:
    KinectFaceTracker(ITracker* parent=NULL, int id=0) : 
          m_parent(parent), 
          m_hWnd(NULL), 
          m_pKinectSensor(NULL), 
          m_pFaceTracker(NULL), 
          m_pFTResult(NULL), 
          m_colorImage(NULL), 
          m_depthImage(NULL),
          m_pImageBuffer(NULL),
          m_pVideoBuffer(NULL),
          m_pCriticalSection(NULL),
          m_id(id) {};
    ~KinectFaceTracker();
    virtual bool                Init();
    virtual bool                Start();
    virtual IArgs*              GetTrackingResults(IArgs* args=NULL);
    virtual void                PaintEvent(void *message, IArgs* args=NULL);
    virtual void                TrackEvent(void *message, IArgs* args=NULL);
    virtual void*               GetCriticalSection() { return static_cast<void*>(&m_pCriticalSection); };

    HRESULT                     Stop();
    HRESULT                     GetTrackerResult();
    void                        CheckCameraInput();
    int                         GetId()             { return(m_id);};
    double                      GetPitch()          { return(pitch);};
    IFTResult*                  GetResult()         { return(m_pFTResult);};
    IFTImage*                   GetColorImage()     { return(m_colorImage);};
    double                      GetXCenterFace()    { return(m_XCenterFace);};
    double                      GetYCenterFace()    { return(m_YCenterFace);};
    BOOL                        IsMaskDraw()        { return(m_config.m_DrawMask);};
    IFTFaceTracker*             GetTracker()        { return(m_pFaceTracker);};
    KinectSensor*               GetSensor()         { return(m_pKinectSensor);};
    BOOL                        IsKinectPresent()   { return(IsKinectSensorPresent);};
    HWND                        GetWindow()         { return m_hWnd;};
    void                        SetWindow(HWND hWnd){ m_hWnd = hWnd;};
    HRESULT                     GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig);
    IAvatar*                    GetAvatar()         { return m_pKinectSensor != NULL ? m_pKinectSensor->GetEggAvatar() : NULL;};
    double                      GetFaceConfidence() { return m_faceConfidence; };
    static DWORD WINAPI         FaceTrackingStaticThread(PVOID lpParam);
    HANDLE                      GetThreadId()       { return m_hFaceTrackingThread; };
    BOOL                        ShowVideo(HDC hdc, int width, int height, int originX, int originY);
    BOOL                        ShowEggAvatar(HDC hdc, int width, int height, int originX, int originY);
    BOOL                        PaintWindow(HDC hdc, HWND hWnd);
    void                        UpdateAvatarPose();
    bool                        m_LastTrackSucceeded;
    bool                        m_ApplicationIsRunning;
    static void FaceTrackerCallback(void* instance=NULL, IArgs* args=NULL);
protected:
    

    CRITICAL_SECTION*           m_pCriticalSection;

    TrackerArgs                 m_trackingResults;
    IFTImage*                   m_pImageBuffer;
    IFTImage*                   m_pVideoBuffer;
    ITracker*                   m_parent;
    int                         m_id;
    KinectSensor*               m_pKinectSensor;
    HANDLE                      m_hFaceTrackingThread;
    IFTFaceTracker*             m_pFaceTracker;
    IFTResult*                  m_pFTResult;
    IFTImage*                   m_colorImage;
    IFTImage*                   m_depthImage;
    
    FT_VECTOR3D                 m_hint3D[2]; 
    
    double                      m_faceConfidence;

    bool                        IsKinectSensorPresent;
    double                      m_XCenterFace;
    double                      m_YCenterFace;
    double                      pitch, yaw, roll;
    double                      scale;
    double                      rotationXYZ[3];
    double                      translationXYZ[3];
    
    HWND                        m_hWnd;
    TrackerConfig               m_config;
    TrackerArgs                 m_args;

    BOOL                        SubmitFraceTrackingResult(IFTResult* pResult);
    void                        SetCenterOfImage(IFTResult* pResult);
    DWORD                        WINAPI FaceTrackingThread();    
};

