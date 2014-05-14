//------------------------------------------------------------------------------
// <copyright file="KinectFaceTracker.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "KinectFaceTracker.h"
#include "TrackerException.h"
#include "Visualize.h"

bool KinectFaceTracker::Init()
{
	m_pKinectSensor = new KinectSensor();
	HRESULT hr1 = m_pKinectSensor->Init(m_config.m_depthType, 
										m_config.m_depthRes,
										m_config.m_bNearMode,
										m_config.m_bFallbackToDefault,
										m_config.m_colorType,
										m_config.m_colorRes,
										m_config.m_bSeatedSkeletonMode);
	
	IsKinectSensorPresent = SUCCEEDED(hr1);
	
	FT_CAMERA_CONFIG videoConfig;
    FT_CAMERA_CONFIG depthConfig;
    FT_CAMERA_CONFIG* pDepthConfig = NULL;

	if (IsKinectSensorPresent)
    {        
        m_pKinectSensor->GetVideoConfiguration(&videoConfig);
        m_pKinectSensor->GetDepthConfiguration(&depthConfig);
        pDepthConfig = &depthConfig;
        m_hint3D[0] = m_hint3D[1] = FT_VECTOR3D(0, 0, 0);
    }
    else
    {
		throw new TrackerInitializationException("Could not detect the Kinect sensor");
		return false;
    }

	// Try to start the face tracker.
    m_pFaceTracker = FTCreateFaceTracker();
    if (!m_pFaceTracker)
    {
		throw new TrackerInitializationException("Could not create the face tracker");
		return false;
    }

    HRESULT hr = m_pFaceTracker->Initialize(&videoConfig, pDepthConfig, NULL, NULL); 
    if (FAILED(hr))
    {
		throw new TrackerInitializationException("Could not initilize the face tracker video config");
        return false;
    }

    hr = m_pFaceTracker->CreateFTResult(&m_pFTResult);
    if (FAILED(hr) || !m_pFTResult)
    {
        throw new TrackerInitializationException("Could not create face tracker result");
        return false;
    }

    // Initialize the RGB image.
    m_colorImage = FTCreateImage();
    if (!m_colorImage || FAILED(hr = m_colorImage->Allocate(videoConfig.Width, videoConfig.Height, FTIMAGEFORMAT_UINT8_B8G8R8X8)))
    {
        throw new TrackerInitializationException("Could not initilize the face tracker RGB image");
		return false;
    }

    if (pDepthConfig)
    {
        m_depthImage = FTCreateImage();
        if (!m_depthImage || FAILED(hr = m_depthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
        {
            throw new TrackerInitializationException("Could not initilize the face tracker depth image");
			return false;
        }
    }

	return IsKinectSensorPresent;
}

KinectFaceTracker::~KinectFaceTracker()
{
    Stop();
	if (m_pFaceTracker)
	{
		m_pFaceTracker->Release();
		m_pFaceTracker = NULL;
	}

    if(m_colorImage)
    {
        m_colorImage->Release();
        m_colorImage = NULL;
    }

    if(m_depthImage) 
    {
        m_depthImage->Release();
        m_depthImage = NULL;
    }

    if(m_pFTResult)
    {
        m_pFTResult->Release();
        m_pFTResult = NULL;
    }
	
	if(m_pKinectSensor)
	{
		delete m_pKinectSensor;
		m_pKinectSensor = NULL;
	}
}

 
BOOL KinectFaceTracker::SubmitFraceTrackingResult(IFTResult* pResult)
{
    if (pResult != NULL && SUCCEEDED(pResult->GetStatus()))
    {
        if (m_CallBack)
        {
            (*m_CallBack)(m_CallBackParam);
        }

        if (IsMaskDraw())
        {
            FLOAT* pSU = NULL;
            UINT numSU;
            BOOL suConverged;
            m_pFaceTracker->GetShapeUnits(NULL, &pSU, &numSU, &suConverged);
            POINT viewOffset = {0, 0};
            FT_CAMERA_CONFIG cameraConfig;

            if (IsKinectSensorPresent)
            {
                m_pKinectSensor->GetVideoConfiguration(&cameraConfig);
            }
            else
            {
                cameraConfig.Width = 640;
                cameraConfig.Height = 480;
                cameraConfig.FocalLength = 500.0f;
            }
            IFTModel* ftModel;
            HRESULT hr = m_pFaceTracker->GetFaceModel(&ftModel);
            if (SUCCEEDED(hr))
            {
                hr = VisualizeFaceModel(m_colorImage, ftModel, &cameraConfig, pSU, 1.0, viewOffset, pResult, 0x00FFFF00);
                ftModel->Release();
            }
        }
    }
    return TRUE;
}

// We compute here the nominal "center of attention" that is used when zooming the presented image.
void KinectFaceTracker::SetCenterOfImage(IFTResult* pResult)
{
    float centerX = ((float)m_colorImage->GetWidth())/2.0f;
    float centerY = ((float)m_colorImage->GetHeight())/2.0f;
    if (pResult)
    {
        if (SUCCEEDED(pResult->GetStatus()))
        {
            RECT faceRect;
            pResult->GetFaceRect(&faceRect);
            centerX = (faceRect.left+faceRect.right)/2.0f;
            centerY = (faceRect.top+faceRect.bottom)/2.0f;
        }
        m_XCenterFace += 0.02f*(centerX-m_XCenterFace);
        m_YCenterFace += 0.02f*(centerY-m_YCenterFace);
    }
    else
    {
        m_XCenterFace = centerX;
        m_YCenterFace = centerY;
    }
}

// Get a video image and process it.


HRESULT KinectFaceTracker::GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig)
{
	return IsKinectSensorPresent ? m_pKinectSensor->GetVideoConfiguration(cameraConfig) : E_FAIL;
}

HRESULT KinectFaceTracker::Stop()
{
    m_ApplicationIsRunning = false;
    if (m_hFaceTrackingThread)
    {
        WaitForSingleObject(m_hFaceTrackingThread, INFINITE);
    }
    m_hFaceTrackingThread = 0;
    return S_OK;
}


bool KinectFaceTracker::Start()
{
	m_hFaceTrackingThread = CreateThread(NULL, 0, KinectFaceTracker::FaceTrackingStaticThread, (void*) this, 0, 0);
	return true;
}


DWORD WINAPI KinectFaceTracker::FaceTrackingStaticThread(PVOID lpParam)
{
	KinectFaceTracker* pthis = static_cast<KinectFaceTracker*>(lpParam);
    if (pthis)
    {
        return pthis->FaceTrackingThread();
    }
    return 0;
}


DWORD WINAPI KinectFaceTracker::FaceTrackingThread()
{     
	while (m_ApplicationIsRunning)
    {	
		HRESULT hrFT = GetTrackerResult();				
		auto trackingStatus = m_pFTResult->GetStatus();
		m_LastTrackSucceeded = SUCCEEDED(hrFT) && SUCCEEDED(trackingStatus);
		if (m_LastTrackSucceeded)
		{
			SetCenterOfImage(m_pFTResult);
			EggAvatar* avatar = m_pKinectSensor->GetEggAvatar();
			avatar->GetRotations(&pitch, &yaw, &roll);
		}

		CheckCameraInput();			
		if (m_hWnd)
		{			
			InvalidateRect(m_hWnd, NULL, FALSE);
			UpdateWindow(m_hWnd);
		}
		Sleep(16);
	}
							    
    return 0;
}

HRESULT KinectFaceTracker::GetTrackerResult()
{
	HRESULT hrFT = E_FAIL;
    if (IsKinectSensorPresent)
    {
        HRESULT hrCopy = m_pKinectSensor->GetVideoBuffer()->CopyTo(m_colorImage, NULL, 0, 0);
        if (SUCCEEDED(hrCopy) && m_pKinectSensor->GetDepthBuffer())
        {
            hrCopy = m_pKinectSensor->GetDepthBuffer()->CopyTo(m_depthImage, NULL, 0, 0);
        }
        // Do face tracking
        if (SUCCEEDED(hrCopy))
        {
            FT_SENSOR_DATA sensorData(m_colorImage, m_depthImage, m_pKinectSensor->GetZoomFactor(), m_pKinectSensor->GetViewOffSet());

            FT_VECTOR3D* hint = NULL;
            if (SUCCEEDED(m_pKinectSensor->GetClosestHint(m_hint3D)))
            {
                hint = m_hint3D;
            }
            if (m_LastTrackSucceeded)
            {
                hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
            }
            else
            {
                hrFT = m_pFaceTracker->StartTracking(&sensorData, NULL, hint, m_pFTResult);
            }

			RECT roi;
			HRESULT hr = m_pFTResult->GetFaceRect(&roi);
			bool  hasFoundFace = (SUCCEEDED(hr) && (roi.bottom != roi.top && roi.left != roi.right));
			FT_WEIGHTED_RECT* faceConfidence = new FT_WEIGHTED_RECT();
			UINT pFaceCount = 1;
			hr = m_pFaceTracker->DetectFaces(&sensorData, hasFoundFace ? &roi : NULL, faceConfidence, &pFaceCount);
			if (SUCCEEDED(hr) && hasFoundFace)
			{
				m_faceConfidence = faceConfidence->Weight;
			}
			delete faceConfidence;
        }
    }

	return hrFT;
}


// Get a video image and process it.
void KinectFaceTracker::CheckCameraInput()
{
    if (m_LastTrackSucceeded)
    {
        SubmitFraceTrackingResult(m_pFTResult);
    }
    else
    {		
        m_pFTResult->Reset();
    }
    SetCenterOfImage(m_pFTResult);
}

