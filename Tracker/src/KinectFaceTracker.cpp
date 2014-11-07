//------------------------------------------------------------------------------
// <copyright file="KinectFaceTracker.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "KinectFaceTracker.h"
#include "TrackerException.h"
#include "Visualize.h"
#include "ICallable.h"
#include "KinectTracker.h"
#include "TrackerHelper.h"

using namespace TrackerHelper;

bool KinectFaceTracker::Init(IArgs* args)
{
    if (!KinectSensor::Init(args))
    {
        throw new TrackerInitializationException("Could not detect the Kinect sensor");
        return false;
    }

    args->Set("trackerId", &m_id);
    SetCallback(FaceTrackerCallback, this, args);

    m_pFaceTracker = FTCreateFaceTracker();
    if (!m_pFaceTracker)
    {
        throw new TrackerInitializationException("Could not create the face tracker");
        return false;
    }

    /*m_hint3D[Current][Head] = m_hint3D[Current][Neck] = FT_VECTOR3D(0 , 0, 0);
    m_hint3D[Previous][Head] = m_hint3D[Previous][Neck] = FT_VECTOR3D(0 , 0, 0);
    m_maskColor = 0xFFFF00;
    m_LastTrackSucceeded = false;*/

    m_pImageBuffer = FTCreateImage();
    m_pVideoBuffer = FTCreateImage();
    
    FT_CAMERA_CONFIG videoConfig;
    FT_CAMERA_CONFIG depthConfig;
    GetVideoConfiguration(&videoConfig);
    GetDepthConfiguration(&depthConfig);
    
    HRESULT hr = m_pFaceTracker->Initialize(&videoConfig, &depthConfig, NULL, NULL); 
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
    m_startRect.top = videoConfig.Height/2 - videoConfig.Height/16;
    m_startRect.bottom = videoConfig.Height/2 + videoConfig.Height/16;
    m_startRect.left = videoConfig.Width/2 - videoConfig.Height/16;
    m_startRect.right = videoConfig.Width/2 + videoConfig.Height/16;

    SetCenterOfImage(NULL);

    FT_CAMERA_CONFIG* pDepthConfig = &depthConfig;
    if (pDepthConfig)
    {
        m_depthImage = FTCreateImage();
        if (!m_depthImage || FAILED(hr = m_depthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
        {
            throw new TrackerInitializationException("Could not initilize the face tracker depth image");
            return false;
        }
    }

    //KinectTracker* pTracker = static_cast<KinectTracker*>(m_parent);
    //if (pTracker)
    //{
    //    m_pCriticalSection = static_cast<CRITICAL_SECTION*>(pTracker->GetCriticalSection());
    //}
    m_messageQueue.Init(args);
    return true;
}


KinectFaceTracker::~KinectFaceTracker()
{
    if(SUCCEEDED(Stop()))
    {
        Release();
    }
}

bool KinectFaceTracker::Release()
{
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

    //if(m_pKinectSensor)
    //{
    //    delete m_pKinectSensor;
    //    m_pKinectSensor = NULL;
    //}

    return KinectController::Release();
}

 
BOOL KinectFaceTracker::SubmitFraceTrackingResult(IFTResult* pResult)
{
    if (pResult != NULL)
    {
        if (SUCCEEDED(m_trackingStatus))
        {
            ICallback callback = GetCallback();
            if (callback)
            {
                Call();
            }
        }

        if (IsMaskDraw())
        {
            FLOAT* pSU = NULL;
            UINT numSU;
            BOOL suConverged;
            m_pFaceTracker->GetShapeUnits(NULL, &pSU, &numSU, &suConverged);
            FT_CAMERA_CONFIG cameraConfig;
            if (FAILED(GetVideoConfiguration(&cameraConfig)))
            {
                cameraConfig.Width = 640;
                cameraConfig.Height = 480;
                cameraConfig.FocalLength = 500.0f;
            }
            IFTModel* ftModel;
            HRESULT hr = m_pFaceTracker->GetFaceModel(&ftModel);
            if (SUCCEEDED(hr))
            {
                hr = VisualizeFaceModel(m_colorImage, ftModel, &cameraConfig, pSU, 1.0, m_viewOffset, pResult, m_maskColor);
                ftModel->Release();
            }
        }
    }
    return TRUE;
}

// We compute here the nominal "center of attention" that is used when zooming the presented image.
void KinectFaceTracker::SetCenterOfImage(IFTResult* pResult)
{
    double centerX = m_colorImage->GetWidth()/2.0;
    double centerY = m_colorImage->GetHeight()/2.0;
    if (pResult)
    {
        if (SUCCEEDED(pResult->GetStatus()))
        {
            RECT faceRect;
            pResult->GetFaceRect(&faceRect);
            centerX = (faceRect.left+faceRect.right)/2.0;
            centerY = (faceRect.top+faceRect.bottom)/2.0;
        }
        m_XCenterFace += 0.02 *(centerX-m_XCenterFace);
        m_YCenterFace += 0.02 *(centerY-m_YCenterFace);
    }
    else
    {
        m_XCenterFace = centerX;
        m_YCenterFace = centerY;
    }
}

bool KinectFaceTracker::Stop()
{
    if (m_parent)
    {
        m_parent = NULL;
    }
    if (m_hFaceTrackingThread)
    {
        WaitForSingleObject(m_hFaceTrackingThread, 5000/*INFINITE*/);
        m_ApplicationIsRunning = false;
    }
    m_hFaceTrackingThread = 0;
    return KinectController::Stop();
}


bool KinectFaceTracker::Start(IArgs* args)
{
    m_hFaceTrackingThread = CreateThread(NULL, 0, KinectFaceTracker::FaceTrackingStaticThread, (void*) this, 0, 0);
    SetThreadPriority(m_hFaceTrackingThread, THREAD_PRIORITY_HIGHEST);
    m_messageQueue.Start();
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
        m_trackingStatus = m_pFTResult->GetStatus();
        m_LastTrackSucceeded = SUCCEEDED(hrFT) && SUCCEEDED(m_trackingStatus);
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
    if (m_bSensorRunning)
    {
        HRESULT hrCopy = GetVideoBuffer()->CopyTo(m_colorImage, NULL, 0, 0);
        if (SUCCEEDED(hrCopy) && GetDepthBuffer())
        {
            hrCopy = GetDepthBuffer()->CopyTo(m_depthImage, NULL, 0, 0);
        }
        // Do face tracking
            if (SUCCEEDED(hrCopy))
        {
            FT_SENSOR_DATA sensorData(m_colorImage, m_depthImage, GetZoomFactor(), GetViewOffSet());

            FT_VECTOR3D hint[2];
            if (SUCCEEDED(GetClosestHint(hint)))
            {
                m_hint3D[Previous][Neck] = m_hint3D[Current][Neck];
                m_hint3D[Previous][Head] = m_hint3D[Previous][Head];
                m_hint3D[Current][Neck] = hint[Neck];
                m_hint3D[Current][Head] = hint[Head];
            }
            if (m_LastTrackSucceeded)
            {
                hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
                //if (FAILED(hrFT))
                //{
                //    hint[Neck] = AddFtVector3d(hint[Neck], SubFtVector3d(m_hint3D[Current][Neck],m_hint3D[Previous][Neck]));
                //    hint[Head] = AddFtVector3d(hint[Head], SubFtVector3d(m_hint3D[Current][Head],m_hint3D[Previous][Head]));
                //    hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
                //}
            }
            else
            {
                hrFT = m_pFaceTracker->StartTracking(&sensorData, NULL, hint, m_pFTResult);
            }

            HRESULT hr = m_pFTResult->GetFaceRect(&m_Roi);
            bool  hasFoundFace = SUCCEEDED(hr) && SUCCEEDED(m_pFTResult->GetStatus());
            if(hasFoundFace)
            {
                FT_WEIGHTED_RECT* faceConfidence = new FT_WEIGHTED_RECT();
                UINT pFaceCount = 1;
                hr = m_pFaceTracker->DetectFaces(&sensorData, hasFoundFace ? &m_startRect : NULL, faceConfidence, &pFaceCount);
                if (SUCCEEDED(hr))
                {
                    m_faceConfidence = faceConfidence->Weight;
                }
                delete faceConfidence;
            }
        }
    }

    return hrFT;
}


// Get a video image and process it.
void KinectFaceTracker::CheckCameraInput()
{
    m_maskColor = GetErrorColor(m_trackingStatus);
    if (m_LastTrackSucceeded)
    {
        SubmitFraceTrackingResult(m_pFTResult);
    } 
    else
    {
        if (!IsFTErrorSevere(m_trackingStatus))
        {
            m_viewOffset.x = std::lround(m_hint3D[Current][Head].x); // lround defined at the top
            m_viewOffset.y = std::lround(m_hint3D[Current][Head].y);
        }
        m_pFTResult->Reset();
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
    if (m_bSensorRunning && m_ApplicationIsRunning)
    {
        errCount += !ShowVideo(hdc, width - halfWidth, height, halfWidth, 0);
        // Draw the egg avatar on the left of the window
        errCount += !ShowEggAvatar(hdc, halfWidth, height, 0, 0);
    }
    
    return ret;
}

void KinectFaceTracker::TrackEvent(IArgs* args)
{ 
    IFTResult* pResult = GetResult();
    if (args)
    {
        args->Set(GetEggAvatar()->ResultsArg, pResult);
    }

    if (pResult && SUCCEEDED(pResult->GetStatus()))
    {
        
        if (m_parent)
        {
            m_parent->TrackEvent(args);
        }
    }
    
    KinectSensor::TrackEvent(args);
}

void KinectFaceTracker::FaceTrackerCallback(void* instance, IArgs* args)
{
    KinectFaceTracker* pThis = reinterpret_cast<KinectFaceTracker*>((void*)instance);
    if (pThis)
    {
        pThis->TrackEvent(args);
    }
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
    if (m_pImageBuffer && SUCCEEDED(m_pImageBuffer->Allocate(width, height, FTIMAGEFORMAT_UINT8_B8G8R8A8)))
    {
        memset(m_pImageBuffer->GetBuffer(), 0, m_pImageBuffer->GetStride() * height); // clear to black
        
        EggAvatar* pEggAvatar = GetEggAvatar();
        if (pEggAvatar)
        {
            pEggAvatar->SetScaleAndTranslationToWindow(height, width);
            pEggAvatar->DrawImage(static_cast<PVOID>(m_pImageBuffer));
        }

        BITMAPINFO bmi = {sizeof(BITMAPINFO), width, height, 1, static_cast<WORD>(m_pImageBuffer->GetBytesPerPixel() * CHAR_BIT), BI_RGB, m_pImageBuffer->GetStride() * height, 5000, 5000, 0, 0};
        errCount += (0 == StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, m_pImageBuffer->GetBuffer(), &bmi, DIB_RGB_COLORS, SRCCOPY));

        ret = TRUE;
    }

    return ret;
}