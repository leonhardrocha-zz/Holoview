//------------------------------------------------------------------------------
// <copyright file="FTHelper.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#pragma warning(disable:4786)
#include <FaceTrackLib.h>
#include "KinectSensor.h"
#include "ITracker.h"
#include "Callable.h"
#include "TrackingResults.h"
#include <vector>
#include <queue>
#include <map>



class TrackerConfig
{
public:	
	TrackerConfig() :
		m_DrawMask(TRUE),
		m_depthType(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX),
		m_depthRes(NUI_IMAGE_RESOLUTION_320x240),
		m_bNearMode(TRUE),
		m_bFallbackToDefault(TRUE),
		m_bSeatedSkeletonMode(TRUE),
		m_colorType(NUI_IMAGE_TYPE_COLOR),	
		m_colorRes(NUI_IMAGE_RESOLUTION_1280x960)
	{}
	BOOL                 m_DrawMask;
	NUI_IMAGE_TYPE       m_depthType;
	NUI_IMAGE_RESOLUTION m_depthRes;
	BOOL                 m_bNearMode;
	BOOL                 m_bFallbackToDefault;
	BOOL                 m_bSeatedSkeletonMode;
	NUI_IMAGE_TYPE       m_colorType;
	NUI_IMAGE_RESOLUTION m_colorRes;
};

class KinectFaceTracker : public ITracker
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
		  m_id(id)
		  {};	
    ~KinectFaceTracker();
	HRESULT			Stop();
	HRESULT			GetTrackerResult();
	bool			Start();
	bool			Init();
	void			CheckCameraInput();
	int   			GetId()			  { return(m_id);};
	float   		GetPitch()         { return(pitch);};
	IFTResult*		GetResult()        { return(m_pFTResult);};
    IFTImage*		GetColorImage()    { return(m_colorImage);};
    float			GetXCenterFace()   { return(m_XCenterFace);};
    float			GetYCenterFace()   { return(m_YCenterFace);};
    BOOL			IsMaskDraw()	   { return(m_config.m_DrawMask);};
    IFTFaceTracker* GetTracker()       { return(m_pFaceTracker);};
	KinectSensor*   GetSensor()        { return(m_pKinectSensor);};
    BOOL			IsKinectPresent()  { return(IsKinectSensorPresent);};
	HWND			GetWindow()			{ return m_hWnd;};
	void		    SetWindow(HWND hWnd){ m_hWnd = hWnd;};
    HRESULT			GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig);
	IAvatar*		GetAvatar()			{ return m_pKinectSensor != NULL ? m_pKinectSensor->GetEggAvatar() : NULL;};
	float			GetFaceConfidence() { return m_faceConfidence; };
	TrackingResults*	GetTrackingResults (TrackingArgs args=NULL);
	void			PaintEvent(void *message, TrackingArgs args=NULL);
	void			TrackEvent(void *message, TrackingArgs args=NULL);
	static void		FTCallback(void* param, TrackingArgs args=NULL);
	static DWORD WINAPI			FaceTrackingStaticThread(PVOID lpParam);	
	HANDLE						GetThreadId() { return m_hFaceTrackingThread; };		
	BOOL            ShowVideo(HDC hdc, int width, int height, int originX, int originY);
    BOOL            ShowEggAvatar(HDC hdc, int width, int height, int originX, int originY);
	BOOL			PaintWindow(HDC hdc, HWND hWnd);
	void			UpdateAvatarPose();
	bool						m_LastTrackSucceeded;
	bool						m_ApplicationIsRunning;
	void*			GetCriticalSection() { return static_cast<void*>(m_pCriticalSection); };
protected:
	CRITICAL_SECTION*			m_pCriticalSection;

	TrackingResults				m_View;
	IFTImage*                   m_pImageBuffer;
    IFTImage*                   m_pVideoBuffer;
	ITracker*					m_parent;
	int							m_id;
	KinectSensor*               m_pKinectSensor;
	HANDLE						m_hFaceTrackingThread;	
	IFTFaceTracker*             m_pFaceTracker;
    IFTResult*                  m_pFTResult;
    IFTImage*                   m_colorImage;
    IFTImage*                   m_depthImage;
	
    FT_VECTOR3D                 m_hint3D[2]; 
	
	float						m_faceConfidence;

	bool						IsKinectSensorPresent;
    float                       m_XCenterFace;
    float                       m_YCenterFace;
	float						pitch, yaw, roll;
	float						scale;
	float						rotationXYZ[3];
	float						translationXYZ[3];
    
	HWND						m_hWnd;
	TrackerConfig				m_config;
    ArgsMap                     m_args;

    BOOL						SubmitFraceTrackingResult(IFTResult* pResult);
    void						SetCenterOfImage(IFTResult* pResult);
	DWORD						WINAPI FaceTrackingThread();	
};

