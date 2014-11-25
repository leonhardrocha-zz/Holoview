//------------------------------------------------------------------------------
// <copyright file="MultiFTHelper.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "MultiFTHelper.h"
#include "Visualize.h"

#ifdef SAMPLE_OPTIONS
#include "multiOptions.h"
#else
PVOID _multiopt = NULL;
#endif

MultiFTHelper::MultiFTHelper()
{
	master = this;
    m_pFaceTracker = 0;
    m_hWnd = NULL;
    m_pFTResult = NULL;
    m_colorImage = NULL;
    m_depthImage = NULL;
    m_ApplicationIsRunning = false;
    m_LastTrackSucceeded = false;
    m_CallBack = NULL;
    m_XCenterFace = 0;
    m_YCenterFace = 0;
    m_hFaceTrackingThread = NULL;
    m_DrawMask = TRUE;
    m_depthType = NUI_IMAGE_TYPE_DEPTH;
    m_depthRes = NUI_IMAGE_RESOLUTION_INVALID;
    m_bNearMode = TRUE;
    m_bFallbackToDefault = FALSE;
    m_colorType = NUI_IMAGE_TYPE_COLOR;
    m_colorRes = NUI_IMAGE_RESOLUTION_INVALID;
	m_bSeatedSkeletonMode = true;
	m_pBestSensorTrackerPair = NULL;
}

MultiFTHelper::MultiFTHelper(MultiFTHelper* parent)
{
	master = parent;
    m_pFaceTracker = parent->m_pFaceTracker;
    m_hWnd = parent->m_hWnd;
    m_pFTResult = parent->m_pFTResult;
    m_colorImage = NULL;
    m_depthImage = NULL;
    m_ApplicationIsRunning = parent->m_ApplicationIsRunning;
    m_LastTrackSucceeded = parent->m_LastTrackSucceeded;
    m_CallBack = parent->m_CallBack;
	m_CallBackParam = parent->m_CallBackParam;
	m_hFaceTrackingThread = parent->m_hFaceTrackingThread;
    m_XCenterFace = parent->m_XCenterFace;
    m_YCenterFace = parent->m_YCenterFace;
	m_DrawMask = parent->m_DrawMask;
    m_depthType = parent->m_depthType;
    m_depthRes = parent->m_depthRes;
    m_bNearMode = parent->m_bNearMode;
    m_bFallbackToDefault = parent->m_bFallbackToDefault;
    m_colorType = parent->m_colorType;
    m_colorRes = parent->m_colorRes;
	m_bSeatedSkeletonMode = parent->m_bSeatedSkeletonMode;
	m_pBestSensorTrackerPair = NULL;
}


MultiFTHelper::~MultiFTHelper()
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
	
	while(!validSensors.empty())
	{
		auto pair = validSensors.begin();
		if(pair->first)
		{
			delete pair->first;
		}
		if(pair->second)
		{
			delete pair->second;
		}		
		validSensors.erase(pair);
	}
}

BOOL MultiFTHelper::SubmitFraceTrackingResult(IFTResult* pResult)
{
    if (pResult != NULL && SUCCEEDED(pResult->GetStatus()))
    {
        if (m_CallBack)
        {
            (*m_CallBack)(m_CallBackParam);
        }

        if (m_DrawMask)
        {
            FLOAT* pSU = NULL;
            UINT numSU;
            BOOL suConverged;
            m_pFaceTracker->GetShapeUnits(NULL, &pSU, &numSU, &suConverged);
            POINT viewOffset = {0, 0};
            FT_CAMERA_CONFIG cameraConfig;

            if (m_KinectSensorPresent)
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

bool MultiFTHelper::SortTrackerPair(const TrackerPair& pair1, const TrackerPair& pair2)
{
	return pair1.second->m_faceConfidence.Weight > pair2.second->m_faceConfidence.Weight;
}

TrackerPair MultiFTHelper::GetBestTracker()
{ 	 
	if(m_pBestSensorTrackerPair != NULL)
	{
		return *m_pBestSensorTrackerPair;
	}

	 if( validSensors.size() > 0)
	 {
		std::sort(validSensors.begin(), validSensors.end(), MultiFTHelper::SortTrackerPair);
		auto best = validSensors.begin(); 
		return TrackerPair(best->first, best->second);
	 }

	 return TrackerPair(m_pKinectSensor, this);
}

// We compute here the nominal "center of attention" that is used when zooming the presented image.
void MultiFTHelper::SetCenterOfImage(IFTResult* pResult)
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


DWORD WINAPI MultiFTHelper::FaceTrackingStaticThread(PVOID lpParam)
{
    MultiFTHelper* context = static_cast<MultiFTHelper*>(lpParam);
    if (context)
    {
        return context->FaceTrackingThread();
    }
    return 0;
}



HRESULT MultiFTHelper::GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig)
{
	return m_KinectSensorPresent ? m_pKinectSensor->GetVideoConfiguration(cameraConfig) : E_FAIL;
}

HRESULT MultiFTHelper::Init(NUI_IMAGE_TYPE       depthType,
							NUI_IMAGE_RESOLUTION depthRes,
							BOOL                 bNearMode,
							BOOL                 bFallbackToDefault,
							BOOL                 bSeatedSkeletonMode,
							NUI_IMAGE_TYPE       colorType,
							NUI_IMAGE_RESOLUTION colorRes,
							HWND 				 hWnd,
							FTHelperCallBack	 callBack,
							PVOID				 callBackParam)
{      
	if (!callBack)
    {
        return E_INVALIDARG;
    }

    m_ApplicationIsRunning = true;
    m_depthType = depthType;
    m_depthRes = depthRes;
    m_bNearMode = bNearMode;
    m_bFallbackToDefault = bFallbackToDefault;
    m_bSeatedSkeletonMode = bSeatedSkeletonMode;
    m_colorType = colorType;
    m_colorRes = colorRes;
	m_hWnd = m_hWnd;
	m_CallBack = callBack;
    m_CallBackParam = callBackParam;

	int numSensors = 0;	
    NuiGetSensorCount(&numSensors);

	for (int i =0; i < numSensors; i++)
	{
		KinectSensor* sensor = new KinectSensor();
		HRESULT hr1 = sensor->Init(m_depthType, m_depthRes, m_bNearMode, m_bFallbackToDefault, m_colorType, m_colorRes, m_bSeatedSkeletonMode);

		if (SUCCEEDED(hr1))
		{
			auto *trackerSlave = new MultiFTHelper(this);
			trackerSlave->m_pKinectSensor = sensor;
			int trackerInitResult = trackerSlave->StartFaceTracker();
			
			if (trackerInitResult != 0)
			{	
				m_KinectSensorPresent = false;
			}
			else
			{				
				m_KinectSensorPresent = true;
				trackerSlave->SetCenterOfImage(NULL);
				sensor->m_smallestDistance = FLT_MAX;
				validSensors.push_back(TrackerPair(sensor, trackerSlave));
			}			
		}
		else
		{
			delete sensor;
		}
	}
		

    m_hFaceTrackingThread = CreateThread(NULL, 0, FaceTrackingStaticThread, (PVOID)this, 0, 0);	 

    return S_OK;
}

HRESULT MultiFTHelper::Stop()
{
    m_ApplicationIsRunning = false;
    if (m_hFaceTrackingThread)
    {
        WaitForSingleObject(m_hFaceTrackingThread, 1000);
    }
    m_hFaceTrackingThread = 0;
    return S_OK;
}


int MultiFTHelper::StartFaceTracker()
{
	FT_CAMERA_CONFIG videoConfig;
    FT_CAMERA_CONFIG depthConfig;
    FT_CAMERA_CONFIG* pDepthConfig = NULL;

	if (m_KinectSensorPresent)
    {        
        m_pKinectSensor->GetVideoConfiguration(&videoConfig);
        m_pKinectSensor->GetDepthConfiguration(&depthConfig);
        pDepthConfig = &depthConfig;
        m_hint3D[0] = m_hint3D[1] = FT_VECTOR3D(0, 0, 0);
    }
    else
    {
		WCHAR errorText[MAX_PATH];
		ZeroMemory(errorText, sizeof(WCHAR) * MAX_PATH);
		wsprintf(errorText, L"Could not initialize the Kinect sensor; \n");
		MessageBoxW(m_hWnd, errorText, L"Face Tracker Initialization Error\n", MB_OK);
		return 1;
    }

	// Try to start the face tracker.
    m_pFaceTracker = FTCreateFaceTracker(_multiopt);
    if (!m_pFaceTracker)
    {
        MessageBoxW(m_hWnd, L"Could not create the face tracker.\n", L"Face Tracker Initialization Error\n", MB_OK);
        return 2;
    }

    HRESULT hr = m_pFaceTracker->Initialize(&videoConfig, pDepthConfig, NULL, NULL); 
    if (FAILED(hr))
    {
        WCHAR path[512], buffer[1024];
        GetCurrentDirectoryW(ARRAYSIZE(path), path);
        wsprintf(buffer, L"Could not initialize face tracker (%s). hr=0x%x", path, hr);

        MessageBoxW(m_hWnd, /*L"Could not initialize the face tracker.\n"*/ buffer, L"Face Tracker Initialization Error\n", MB_OK);

        return 3;
    }

    hr = m_pFaceTracker->CreateFTResult(&m_pFTResult);
    if (FAILED(hr) || !m_pFTResult)
    {
#ifdef _DEBUG
		auto status = m_pFTResult->GetStatus();
		if(status == FT_ERROR_FACE_DETECTOR_FAILED)
		{
			MessageBoxW(m_hWnd, L"Could not initialize the face tracker result.\n", L"Face Tracker Face Detector Error\n", MB_OK);
		}
		if(status == E_INVALIDARG)
		{
			MessageBoxW(m_hWnd, L"Could not initialize the face tracker result.\n", L"Invalid arguments\n", MB_OK);
		}
		if(status == FT_ERROR_AAM_FAILED)
		{
			MessageBoxW(m_hWnd, L"Could not initialize the face tracker result.\n", L"Face Tracker AAM Failed\n", MB_OK);
		}
		if(status == FT_ERROR_NN_FAILED)
		{
			MessageBoxW(m_hWnd, L"Could not initialize the face tracker result.\n", L"Face Tracker NN Failed\n", MB_OK);
		}
		if(status == FT_ERROR_EVAL_FAILED)
		{
			MessageBoxW(m_hWnd, L"Could not initialize the face tracker result.\n", L"Face Tracker Eval Failed\n", MB_OK);
		}
#endif		
        MessageBoxW(m_hWnd, L"Could not initialize the face tracker result.\n", L"Face Tracker Initialization Error\n", MB_OK);
        return 4;
    }

    // Initialize the RGB image.
    m_colorImage = FTCreateImage();
    if (!m_colorImage || FAILED(hr = m_colorImage->Allocate(videoConfig.Width, videoConfig.Height, FTIMAGEFORMAT_UINT8_B8G8R8X8)))
    {
        return 5;
    }

    if (pDepthConfig)
    {
        m_depthImage = FTCreateImage();
        if (!m_depthImage || FAILED(hr = m_depthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
        {
            return 6;
        }
    }

	return 0;
}


DWORD WINAPI MultiFTHelper::FaceTrackingThread()
{     
	while (m_ApplicationIsRunning && m_KinectSensorPresent)
    {
		for(auto iterator = validSensors.begin();  iterator != validSensors.end(); ++iterator)
		{
			auto tracker = iterator->second;
			HRESULT hrFT = tracker->GetTrackerResult();				
			auto trackingStatus = tracker->m_pFTResult->GetStatus();
			tracker->m_LastTrackSucceeded = SUCCEEDED(hrFT) && SUCCEEDED(trackingStatus);
			if (tracker->m_LastTrackSucceeded)
			{
				tracker->SetCenterOfImage(tracker->m_pFTResult);
			}
			
		}
		m_pBestSensorTrackerPair = NULL; // reset tracker cache
		auto bestSensor = GetBestTracker();
		auto tracker = bestSensor.second;
		
		m_pKinectSensor = bestSensor.first;		
		EggAvatar* avatar = m_pKinectSensor->GetEggAvatar();
		
		avatar->GetRotations(&pitch, &yaw, &roll);
				
		tracker->CheckCameraInput();			
		if (tracker->m_hWnd)
		{
			InvalidateRect(tracker->m_hWnd, NULL, FALSE);
			UpdateWindow(m_hWnd);
		}
		
        Sleep(16);	
    }
	    
    return 0;
}

HRESULT MultiFTHelper::GetTrackerResult()
{
	HRESULT hrFT = E_FAIL;
    if (m_KinectSensorPresent && m_pKinectSensor->GetVideoBuffer())
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
			FT_WEIGHTED_RECT faceConfidence;
			UINT pFaceCount = 1;
			hr = m_pFaceTracker->DetectFaces(&sensorData, hasFoundFace ? &roi : NULL, &faceConfidence, &pFaceCount);
			if (SUCCEEDED(hr))
			{
				m_faceConfidence = faceConfidence;
			}
        }
    }

	return hrFT;
}


// Get a video image and process it.
void MultiFTHelper::CheckCameraInput()
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

