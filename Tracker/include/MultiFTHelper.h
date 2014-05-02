//------------------------------------------------------------------------------
// <copyright file="FTHelper.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#pragma warning(disable:4786)
#include <FaceTrackLib.h>
#include "KinectSensor.h"
#include <vector>
#include <queue>
#include <map>

typedef void (*FTHelperCallBack)(PVOID lpParam);



class MultiFTHelper;

typedef std::pair<KinectSensor*,MultiFTHelper*> TrackerPair;
typedef std::vector<TrackerPair> TrackerPairVector;

class MultiFTHelper
{
	
public:
	MultiFTHelper();
	MultiFTHelper(MultiFTHelper* parent);
    ~MultiFTHelper();
	HRESULT Init(	NUI_IMAGE_TYPE       m_depthType,
					NUI_IMAGE_RESOLUTION m_depthRes,
					BOOL                 m_bNearMode,
					BOOL                 m_bFallbackToDefault,
					BOOL                 m_bSeatedSkeletonMode,
					NUI_IMAGE_TYPE       m_colorType,
					NUI_IMAGE_RESOLUTION m_colorRes,
					HWND 				 hWnd,
					FTHelperCallBack callBack,
					PVOID callBackParam);
	HRESULT Stop();
	HRESULT GetTrackerResult();
	int StartFaceTracker();
	void CheckCameraInput();
	IFTResult*		GetResult()        { return(m_pFTResult);}
    IFTImage*		GetColorImage()    { return(m_colorImage);}
    float			GetXCenterFace()   { return(m_XCenterFace);}
    float			GetYCenterFace()   { return(m_YCenterFace);}
    BOOL			GetDrawMask()      { return(m_DrawMask);}
    IFTFaceTracker* GetTracker()       { return(m_pFaceTracker);}
    BOOL			IsKinectPresent()  { return(m_KinectSensorPresent);}
	void		    SetWindow(HWND hWnd)	{ m_hWnd = hWnd;}
    HRESULT			GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig);
	TrackerPair		GetBestTracker(); 
	FTHelperCallBack     m_CallBack;
    LPVOID               m_CallBackParam;
	HANDLE               m_hFaceTrackingThread;
	TrackerPair*				m_pBestSensorTrackerPair;
	KinectSensor*               m_pKinectSensor;
	protected:


	MultiFTHelper*				master;
	IFTFaceTracker*             m_pFaceTracker;
    IFTResult*                  m_pFTResult;
    IFTImage*                   m_colorImage;
    IFTImage*                   m_depthImage;
    FT_VECTOR3D                 m_hint3D[2]; 
	
	FT_WEIGHTED_RECT			m_faceConfidence;

	bool						m_KinectSensorPresent;
    float                       m_XCenterFace;
    float                       m_YCenterFace;

	TrackerPairVector	 validSensors;

	HWND                 m_hWnd;
	bool                 m_LastTrackSucceeded;
	bool                 m_ApplicationIsRunning;

    BOOL                 m_DrawMask;
    NUI_IMAGE_TYPE       m_depthType;
    NUI_IMAGE_RESOLUTION m_depthRes;
    BOOL                 m_bNearMode;
    BOOL                 m_bFallbackToDefault;
    BOOL                 m_bSeatedSkeletonMode;
    NUI_IMAGE_TYPE       m_colorType;
    NUI_IMAGE_RESOLUTION m_colorRes;

    BOOL SubmitFraceTrackingResult(IFTResult* pResult);
    void SetCenterOfImage(IFTResult* pResult);
    DWORD WINAPI FaceTrackingThread();
	static bool SortTrackerPair(const TrackerPair& pair1, const TrackerPair& pair2);
    static DWORD WINAPI FaceTrackingStaticThread(PVOID lpParam);
	
};

