//------------------------------------------------------------------------------
// <copyright file="KinectFaceTracker.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "KinectFaceTracker.h"
#include "KinectTracker.h"
#include "TrackerException.h"
#include "Visualize.h"
#include "ICallable.h"
#include "TrackerHelper.h"

using namespace TrackerHelper;


KinectSensor* KinectFaceTracker::GetSensor()
{
    if (!m_pKinectSensor)
    {
        throw new TrackerRuntimeException("KinectSensor is NULL");
    }
    return m_pKinectSensor;
}

IFTFaceTracker* KinectFaceTracker::GetFaceTracker()
{
    if (!m_pFaceTracker)
    {
        throw new TrackerRuntimeException("IFTFaceTracker is NULL");
    }
    return m_pFaceTracker;
}

bool KinectFaceTracker::do_init()
{
    m_pFaceTracker=NULL;
    m_pFTResult=NULL;
    m_colorImage=NULL;
    m_depthImage=NULL;
    m_pKinectSensor = new KinectSensor();
    m_id = KinectSensor::NumOfInitilizedSensors-1;
    if (!m_pKinectSensor)
    {
        throw new TrackerInitializationException("Kinect sensor not allocated");
        return false;
    }

    m_pFaceTracker = FTCreateFaceTracker();
    if (!m_pFaceTracker)
    {
        throw new TrackerInitializationException("Could not create the face tracker");
        return false;
    }

    LastTrackSucceeded = false;

    FT_CAMERA_CONFIG videoConfig;
    FT_CAMERA_CONFIG depthConfig;
    m_pKinectSensor->GetVideoConfiguration(&videoConfig);
    m_pKinectSensor->GetDepthConfiguration(&depthConfig);
    FT_CAMERA_CONFIG* pDepthConfig = &depthConfig;
    
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
    //m_messageQueue.Init(args);

    return true;
}

bool KinectFaceTracker::do_release()
{

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
    return true;
}

 

bool KinectFaceTracker::do_stop()
{
    if (m_hFaceTrackingThread)
    {
        WaitForSingleObject(m_hFaceTrackingThread, 5000/*INFINITE*/);
        m_hFaceTrackingThread = NULL;
    }
    if (UpdateCallback.GetInstance())
    {
        UpdateCallback.Set(NULL, NULL);
    }
    return true;
}

bool KinectFaceTracker::GetFaceModel(IFTImage* colorImage)
{
    if (!m_pFTResult)
    {
        throw new TrackerRuntimeException("m_pFTResult is NULL");
        return false;
    }

    FLOAT* pSU = NULL;
    UINT numSU;
    BOOL suConverged;
    GetFaceTracker()->GetShapeUnits(NULL, &pSU, &numSU, &suConverged);
    FT_CAMERA_CONFIG cameraConfig;
    if (FAILED(GetSensor()->GetVideoConfiguration(&cameraConfig)))
    {
        cameraConfig.Width = 640;
        cameraConfig.Height = 480;
        cameraConfig.FocalLength = 500.0f;
    }
    IFTModel* ftModel;
    HRESULT hr = m_pFaceTracker->GetFaceModel(&ftModel);
    if (SUCCEEDED(hr))
    {
        UINT32 maskColor = GetErrorColor(m_trackingStatus);
        POINT viewOffset = *m_pKinectSensor->GetViewOffSet();
        hr = VisualizeFaceModel(colorImage, ftModel, &cameraConfig, pSU, 1.0, viewOffset, m_pFTResult, maskColor);
        ftModel->Release();
    }
    return true;
}

bool KinectFaceTracker::do_start()
{
    m_hFaceTrackingThread = CreateThread(NULL, 0, KinectFaceTracker::FaceTrackingStaticThread, (void*) this, 0, 0);
    SetThreadPriority(m_hFaceTrackingThread, THREAD_PRIORITY_HIGHEST);
    //m_messageQueue.Start();
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
    while (IsRunning())
    {    
        HRESULT hrFT = GetTrackerResult();
        // Get a video image and process it.
        if (LastTrackSucceeded)
        {
            if (SUCCEEDED(m_trackingStatus))
            {
                this->TrackEvent(static_cast<void*>(m_pFTResult));
            }
        }
        if(UpdateCallback.GetInstance())
        {
            UpdateCallback.Call();
        }
#ifdef TRACKER_DEBUG
        if (!LastTrackSucceeded)
        {
                

            std::string msg = "Tracker Error: ";
            if (FAILED(hrFT))
            {
                msg += TrackerHelper::GetFTErrorMessage(hrFT);
                if (FAILED(m_trackingStatus))
                {
                    msg += " and ";
                }
            }
            if (FAILED(m_trackingStatus))
            {
                msg += TrackerHelper::GetFTErrorMessage(m_trackingStatus);;
            }
            std::wstring stemp = std::wstring(msg.begin(), msg.end());
            int msgboxID = MessageBox(
                NULL,
                stemp.c_str(),
                L"Tracker Error",
                MB_ICONEXCLAMATION);
        }
#endif
    }
    return 0;
}

HRESULT KinectFaceTracker::GetTrackerResult()
{
    HRESULT hrFT = E_FAIL;
    KinectSensor* pSensor = GetSensor();
    if (pSensor->IsRunning())
    {
        HRESULT hrCopy = pSensor->GetVideoBuffer()->CopyTo(m_colorImage, NULL, 0, 0);
        if (SUCCEEDED(hrCopy) && pSensor->GetDepthBuffer())
        {
            hrCopy = pSensor->GetDepthBuffer()->CopyTo(m_depthImage, NULL, 0, 0);
        }
        // Do face tracking
        //m_pFTResult->Reset();
        if (SUCCEEDED(hrCopy))
        {
            FT_SENSOR_DATA sensorData(m_colorImage, m_depthImage, pSensor->GetZoomFactor(), pSensor->GetViewOffSet());

            FT_VECTOR3D hint[2];
            bool hasFoundFace = SUCCEEDED(pSensor->GetClosestHint(hint));
            if (LastTrackSucceeded)
            {
                hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
            }
            else
            {
                m_pFTResult->Reset();
                hrFT = m_pFaceTracker->StartTracking(&sensorData, NULL, hint, m_pFTResult);
            }
            
            m_trackingStatus = m_pFTResult->GetStatus();
            LastTrackSucceeded = SUCCEEDED(hrFT) && SUCCEEDED(m_pFTResult->GetStatus());
            
            if(hasFoundFace && LastTrackSucceeded)
            {
                HRESULT hr = m_pFTResult->GetFaceRect(&m_Roi);
                FT_WEIGHTED_RECT* faceConfidence = new FT_WEIGHTED_RECT();
                UINT pFaceCount = 1;
                hr = m_pFaceTracker->DetectFaces(&sensorData, hasFoundFace ? &m_Roi : NULL, faceConfidence, &pFaceCount);
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

void KinectFaceTracker::do_trackEvent(void* message)
{ 
    if (IsRunning())
    {    
        HRESULT hrFT = GetTrackerResult();
        // Get a video image and process it.
        if (LastTrackSucceeded)
        {
            if (SUCCEEDED(m_trackingStatus))
            {
                this->TrackEvent(static_cast<void*>(m_pFTResult));
            }
        }
        if(UpdateCallback.GetInstance())
        {
            UpdateCallback.Call();
        }
    }
}

