//------------------------------------------------------------------------------
// <copyright file="KinectFaceTracker.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "KinectFaceTracker.h"
#include "TrackerException.h"
#include "Visualize.h"
#include "IAvatar.h"

bool KinectFaceTracker::Init()
{
	m_pKinectSensor = new KinectSensor();
	m_pImageBuffer = FTCreateImage();
    m_pVideoBuffer = FTCreateImage();
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
		SetTrackerCallback(FTCallback, this, &m_id);
        m_pKinectSensor->GetVideoConfiguration(&videoConfig);
        m_pKinectSensor->GetDepthConfiguration(&depthConfig);
        pDepthConfig = &depthConfig;
				
		m_View.camera.target[Xaxis] = 0.8f;
		m_View.camera.target[Yaxis] = 1.2f;
		m_View.camera.target[Zaxis] = 1.5f;
		m_View.camera.position[Xaxis] = m_id==1 ?  0.8f : 1.6f;
		m_View.camera.position[Yaxis] = m_id==1 ?  1.3f : 0.8f;
		m_View.camera.position[Zaxis] = m_id==1 ?  0.1f : 0.5f;
		m_View.camera.upVector[Xaxis] = m_id==1 ?  0.0f : 0.0f;
		m_View.camera.upVector[Yaxis] = m_id==1 ?  1.0f : 1.0f;
		m_View.camera.upVector[Zaxis] = m_id==1 ?  0.0f : 0.0f;

		
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
			
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

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
	
	if (m_pImageBuffer)
    {
        m_pImageBuffer->Release();
        m_pImageBuffer = NULL;
    }

    if (m_pVideoBuffer)
    {
        m_pVideoBuffer->Release();
        m_pVideoBuffer = NULL;
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
            (*m_CallBack)(m_CallBackParam, m_CallBackArgs);
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
	if (m_parent)
	{
		m_parent = NULL;
	}
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
			CheckCameraInput();
		}

		
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

void KinectFaceTracker::PaintEvent(void *message, TrackingArgs args)
{
	MSG* msg = reinterpret_cast<MSG*>(message);
	if (msg != NULL)
	{
		PAINTSTRUCT ps;
		if (!m_hWnd)
		{
			m_hWnd = msg->hwnd;
		}
		HDC hdc= BeginPaint(m_hWnd, &ps);
		// Draw the avatar window and the video window
		PaintWindow(hdc, m_hWnd);
		EndPaint(m_hWnd, &ps);
	}
}

// Draw the egg head and the camera video with the mask superimposed.
BOOL KinectFaceTracker::PaintWindow(HDC hdc, HWND hWnd)
{	
    static int errCount = 0;
    BOOL ret = FALSE;
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int halfWidth = width/2;
    // Show the video on the right of the window
	if (IsKinectPresent())
	{
		errCount += !ShowVideo(hdc, width - halfWidth, height, halfWidth, 0);
		// Draw the egg avatar on the left of the window
		errCount += !ShowEggAvatar(hdc, halfWidth, height, 0, 0);
	}
	
    return ret;
}

void KinectFaceTracker::TrackEvent(void *message, TrackingArgs args)
{
	if(m_parent)
	{
		m_parent->TrackEvent(message, args);
	}
}

void KinectFaceTracker::UpdateAvatarPose() 
{ 
	IFTResult* pResult = GetResult();
    if (pResult && SUCCEEDED(pResult->GetStatus()))
    {
        FLOAT* pAU = NULL;
        UINT numAU;
		float rotationXYZ[3];
		float translationXYZ[3];
		float scale;
	    pResult->GetAUCoefficients(&pAU, &numAU);
        pResult->Get3DPose(&scale, rotationXYZ,translationXYZ);
		
		IAvatar* pEggAvatar = GetAvatar();
		if (pEggAvatar)
		{
			pEggAvatar->SetTranslations(translationXYZ[0], translationXYZ[1], translationXYZ[2]);
			pEggAvatar->SetRotations(rotationXYZ[0], rotationXYZ[1], rotationXYZ[2]);
		}
		m_View.avatar.SetAvatarPose(*pEggAvatar->GetPose());
	}
}

void KinectFaceTracker::FTCallback(void* param, TrackingArgs args)
{
    KinectFaceTracker* pThis = reinterpret_cast<KinectFaceTracker*>(param);
    if (pThis)
    {
		pThis->UpdateAvatarPose();
		void *message = static_cast<void*>(pThis->GetTrackingResults(args));
		int *pid = static_cast<int*>(args);
		pThis->TrackEvent(message, pid); 		
    }
}

TrackingResults* KinectFaceTracker::GetTrackingResults(TrackingArgs args)
{
	m_View.UpdateTransforms();
	return &m_View;
}
// Drawing the video window
BOOL KinectFaceTracker::ShowVideo(HDC hdc, int width, int height, int originX, int originY)
{
    // Now, copy a fraction of the camera image into the screen.
	IFTImage* colorImage = GetColorImage();
	if (colorImage)
    {
        int iWidth = colorImage->GetWidth();
        int iHeight = colorImage->GetHeight();
        if (iWidth > 0 && iHeight > 0)
        {
            int iTop = 0;
            int iBottom = iHeight;
            int iLeft = 0;
            int iRight = iWidth;

            // Keep a separate buffer.
            if (m_pVideoBuffer && SUCCEEDED(m_pVideoBuffer->Allocate(iWidth, iHeight, FTIMAGEFORMAT_UINT8_B8G8R8A8)))
            {
                // Copy do the video buffer while converting bytes
                colorImage->CopyTo(m_pVideoBuffer, NULL, 0, 0);

                // Compute the best approximate copy ratio.
                float w1 = (float)iHeight * (float)width;
                float w2 = (float)iWidth * (float)height;
                if (w2 > w1 && height > 0)
                {
                    // video image too wide
                    float wx = w1/height;
                    iLeft = (int)max(0, GetXCenterFace() - wx / 2);
                    iRight = iLeft + (int)wx;
                    if (iRight > iWidth)
                    {
                        iRight = iWidth;
                        iLeft = iRight - (int)wx;
                    }
                }
                else if (w1 > w2 && width > 0)
                {
                    // video image too narrow
                    float hy = w2/width;
                    iTop = (int)max(0, GetYCenterFace() - hy / 2);
                    iBottom = iTop + (int)hy;
                    if (iBottom > iHeight)
                    {
                        iBottom = iHeight;
                        iTop = iBottom - (int)hy;
                    }
                }
                int const bmpPixSize = m_pVideoBuffer->GetBytesPerPixel();
                SetStretchBltMode(hdc, HALFTONE);
                BITMAPINFO bmi = {sizeof(BITMAPINFO), iWidth, iHeight, 1, static_cast<WORD>(bmpPixSize * CHAR_BIT), BI_RGB, m_pVideoBuffer->GetStride() * iHeight, 5000, 5000, 0, 0};
                if (0 == StretchDIBits(hdc, originX, originY, width, height,
                    iLeft, iBottom, iRight-iLeft, iTop-iBottom, m_pVideoBuffer->GetBuffer(), &bmi, DIB_RGB_COLORS, SRCCOPY))
                {
                    return FALSE;
                }
            }
        }
		return TRUE;
    }
    return FALSE;
}

// Drawing code
BOOL KinectFaceTracker::ShowEggAvatar(HDC hdc, int width, int height, int originX, int originY)
{
    static int errCount = 0;
    BOOL ret = FALSE;
	
	IAvatar* pEggAvatar = GetAvatar();
	
	if (m_pImageBuffer && SUCCEEDED(m_pImageBuffer->Allocate(width, height, FTIMAGEFORMAT_UINT8_B8G8R8A8)))
	{
		memset(m_pImageBuffer->GetBuffer(), 0, m_pImageBuffer->GetStride() * height); // clear to black
		
		pEggAvatar->SetScaleAndTranslationToWindow(height, width);
		pEggAvatar->DrawImage(static_cast<PVOID>(m_pImageBuffer));

		BITMAPINFO bmi = {sizeof(BITMAPINFO), width, height, 1, static_cast<WORD>(m_pImageBuffer->GetBytesPerPixel() * CHAR_BIT), BI_RGB, m_pImageBuffer->GetStride() * height, 5000, 5000, 0, 0};
		errCount += (0 == StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, m_pImageBuffer->GetBuffer(), &bmi, DIB_RGB_COLORS, SRCCOPY));

		ret = TRUE;
	}

    return ret;
}