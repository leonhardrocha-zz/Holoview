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
#include "ICallable.h"

FT_VECTOR3D AddFtVector3d(const FT_VECTOR3D& a, const FT_VECTOR3D& b)
{
    FT_VECTOR3D c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

FT_VECTOR3D SubFtVector3d(const FT_VECTOR3D& a, const FT_VECTOR3D& b)
{
    FT_VECTOR3D c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

UINT32 GetErrorColor(HRESULT trackingStatus)
 {
    switch (trackingStatus)
    {
        case FT_ERROR_FACE_DETECTOR_FAILED:
            return 0x000000;
        case FT_ERROR_AAM_FAILED:
            return 0x00F0F0;
        case FT_ERROR_NN_FAILED:
            return 0x00FFFF;
        case FT_ERROR_INVALID_3DHINT:
            return 0x0F0F00;
        case FT_ERROR_EVAL_FAILED:
            return 0xFFFF00;
        case FT_ERROR_HEAD_SEARCH_FAILED:
            return 0x0F0000;
        case FT_ERROR_USER_LOST:
            return 0xFF0000;
        default:
            return 0x00FF00;
    }
 }


 bool IsFTErrorSevere(HRESULT trackingStatus)
 {
    switch (trackingStatus)
    {
        case FT_ERROR_FACE_DETECTOR_FAILED:
        case FT_ERROR_AAM_FAILED:
        case FT_ERROR_NN_FAILED:
        case FT_ERROR_INVALID_3DHINT:
        case FT_ERROR_EVAL_FAILED:
        case FT_ERROR_HEAD_SEARCH_FAILED:
        case FT_ERROR_USER_LOST:
            return false;
        default:
            return true;
    }
    return true;
 }
// Face Tracking Error Codes
//#define FT_FACILITY                             0xFAC   // FT facility error code               // Face Tracking facility error code
//#define FT_ERROR_INVALID_MODELS                 MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 1)    // Returned when the face tracking models loaded by the tracking engine have incorrect format
//#define FT_ERROR_INVALID_INPUT_IMAGE            MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 2)    // Returned when passed input image is invalid
//#define FT_ERROR_FACE_DETECTOR_FAILED           MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 3)    // Returned when face tracking fails due to face detection errors
//#define FT_ERROR_AAM_FAILED                     MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 4)    // Returned when face tracking fails due to errors in tracking individual face parts
//#define FT_ERROR_NN_FAILED                      MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 5)    // Returned when face tracking fails due to inability of the Neural Network to find nose, mouth corners and eyes
//#define FT_ERROR_UNINITIALIZED                  MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 6)    // Returned when uninitialized face tracker is used
//#define FT_ERROR_INVALID_MODEL_PATH             MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 7)    // Returned when a file path to the face model files is invalid or when the model files could not be located
//#define FT_ERROR_EVAL_FAILED                    MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 8)    // Returned when face tracking worked but later evaluation found that the quality of the results was poor
//#define FT_ERROR_INVALID_CAMERA_CONFIG          MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 9)    // Returned when the passed camera configuration is invalid
//#define FT_ERROR_INVALID_3DHINT                 MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 10)   // Returned when the passed 3D hint vectors contain invalid values (for example out of range)
//#define FT_ERROR_HEAD_SEARCH_FAILED             MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 11)   // Returned when the system cannot find the head area in the passed data based on passed 3D hint vectors or region of interest rectangle
//#define FT_ERROR_USER_LOST                      MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 12)   // Returned when the user ID of the subject being tracked is switched or lost so we should call StartTracking on next call for tracking face
//#define FT_ERROR_KINECT_DLL_FAILED              MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 13)   // Returned when Kinect DLL failed to load
//#define FT_ERROR_KINECT_NOT_CONNECTED           MAKE_HRESULT(SEVERITY_ERROR, FT_FACILITY, 14)   // Returned when Kinect sensor was not detected in the system 

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

    if (!IsKinectSensorPresent)
    {
        throw new TrackerInitializationException("Could not detect the Kinect sensor");
        return false;
    }

    m_args.Set("trackerId", &m_id);
    SetCallback(FaceTrackerCallback, this, &m_args);

    m_pFaceTracker = FTCreateFaceTracker();
    if (!m_pFaceTracker)
    {
        throw new TrackerInitializationException("Could not create the face tracker");
        return false;
    }

    m_pKinectSensor->GetVideoConfiguration(&videoConfig);
    m_pKinectSensor->GetDepthConfiguration(&depthConfig);
    pDepthConfig = &depthConfig;
    m_hint3D[Current][Head] = m_hint3D[Current][Neck] = FT_VECTOR3D(0 , 0, 0);
    m_hint3D[Previous][Head] = m_hint3D[Previous][Neck] = FT_VECTOR3D(0 , 0, 0);
    m_maskColor = 0xFFFF00;
    m_LastTrackSucceeded = false;
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
    m_pFTLastResult = NULL;
    hr = m_pFaceTracker->CreateFTResult(&m_pFTLastResult);
    if (FAILED(hr) || !m_pFTLastResult)
    {
        throw new TrackerInitializationException("Could not create face tracker LAST result");
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

    if (pDepthConfig)
    {
        m_depthImage = FTCreateImage();
        if (!m_depthImage || FAILED(hr = m_depthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
        {
            throw new TrackerInitializationException("Could not initilize the face tracker depth image");
            return false;
        }
    }

    if (m_parent)
    {
        m_pCriticalSection = static_cast<CRITICAL_SECTION*>(m_parent->GetCriticalSection());
    }

    m_pKinectController = new KinectPoseController(this, this->m_pKinectSensor);
    m_pKinectController->Init();

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

    if(m_pFTLastResult)
    {
        m_pFTLastResult->Release();
        m_pFTLastResult = NULL;
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

    if (m_pKinectController)
    {
        delete m_pKinectController;
        m_pKinectController = NULL;
    }

    if(m_pKinectSensor)
    {
        delete m_pKinectSensor;
        m_pKinectSensor = NULL;
    }
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
        WaitForSingleObject(m_hFaceTrackingThread, 5000/*INFINITE*/);
    }
    m_hFaceTrackingThread = 0;
    return S_OK;
}


bool KinectFaceTracker::Start()
{
    m_hFaceTrackingThread = CreateThread(NULL, 0, KinectFaceTracker::FaceTrackingStaticThread, (void*) this, 0, 0);
    SetThreadPriority(m_hFaceTrackingThread, THREAD_PRIORITY_HIGHEST);
    if (m_pKinectController) 
    {
        m_pKinectController->Start();
    }
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

            FT_VECTOR3D hint[2];
            if (SUCCEEDED(m_pKinectSensor->GetClosestHint(hint)))
            {
                m_hint3D[Previous][Neck] = m_hint3D[Current][Neck];
                m_hint3D[Previous][Head] = m_hint3D[Previous][Head];
                m_hint3D[Current][Neck] = hint[Neck];
                m_hint3D[Current][Head] = hint[Head];
            }
            if (m_LastTrackSucceeded)
            {
                hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
                if (FAILED(hrFT))
                {
                    hint[Neck] = AddFtVector3d(hint[Neck], SubFtVector3d(m_hint3D[Current][Neck],m_hint3D[Previous][Neck]));
                    hint[Head] = AddFtVector3d(hint[Head], SubFtVector3d(m_hint3D[Current][Head],m_hint3D[Previous][Head]));;
                    hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
                }
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
        m_pFTResult->CopyTo(m_pFTLastResult);
        SetCenterOfImage(m_pFTResult);
        m_viewOffset.x = 0.0;
        m_viewOffset.y = 0.0;
        SubmitFraceTrackingResult(m_pFTResult);
    } 
    else
    {
        if (!IsFTErrorSevere(m_trackingStatus))
        {
            SetCenterOfImage(m_pFTLastResult);
            m_viewOffset.x = m_hint3D[Current][Head].x;
            m_viewOffset.y = m_hint3D[Current][Head].y;
            SubmitFraceTrackingResult(m_pFTLastResult);
        }
        m_pFTResult->Reset();
    }
}

void KinectFaceTracker::PaintEvent(void *message, IArgs* args)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg != NULL)
    {
        PAINTSTRUCT ps;
        if (m_hWnd == NULL)
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

void KinectFaceTracker::TrackEvent(void *message, IArgs* args)
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
        if (m_parent)
        {
            if(m_pCriticalSection)
            {
                EnterCriticalSection(m_pCriticalSection);
            }
            m_trackingResults.Set("position", pEggAvatar->GetPosition());
            m_trackingResults.Set("attitude", pEggAvatar->GetAttitude());
            if (m_pKinectController)
            {
                m_pKinectController->GetTrackingResults();
            }
            if(m_pCriticalSection)
            {
                LeaveCriticalSection(m_pCriticalSection);
            }
        }

    }
}

void KinectFaceTracker::FaceTrackerCallback(void* instance, IArgs* args)
{
    KinectFaceTracker* pThis = reinterpret_cast<KinectFaceTracker*>(instance);
    if (pThis)
    {
        pThis->UpdateAvatarPose();
        void *message = static_cast<void*>(pThis->GetTrackingResults(args));
        pThis->TrackEvent(message, args);
    }
}

IArgs* KinectFaceTracker::GetTrackingResults(IArgs* args)
{
    return &m_trackingResults;
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