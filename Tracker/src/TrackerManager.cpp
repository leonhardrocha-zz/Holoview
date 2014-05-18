//------------------------------------------------------------------------------
// <copyright file="TrackerManager.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Defines the entry point for the application.
//
#include "stdafx.h"
#include "TrackerManager.h"
#include "TrackerException.h"

bool TrackerManager::Start()
{
	for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
	{
		if ((*tracker)->Start())
		{
			m_FaceTrackingThreads.push_back((*tracker)->GetThreadId());					
		}
		else
		{
			return false;
		}
	}
	
	return true;
}

bool TrackerManager::Init()
{      
	int numSensors = 0;	
    NuiGetSensorCount(&numSensors);

	int i;
	for (i =0; i < numSensors; i++)
	{
		KinectFaceTracker* tracker = new KinectFaceTracker(this);
		if (tracker->Init())
		{
			m_pFaceTrackers.push_back(tracker);
		} else
		{
			delete tracker;
		}
	}
			  
    return (i == numSensors);
}

void TrackerManager::UninitInstance()
{
    // Clean up the memory allocated for Face Tracking and rendering.
	for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
	{
		(*tracker)->Stop();		
		delete *tracker;
	}

    if (m_hAccelTable)
    {
        DestroyAcceleratorTable(m_hAccelTable);
        m_hAccelTable = NULL;
    }
}

void TrackerManager::PaintEvent(void *message, TrackingArgs args)
{
	KinectFaceTracker *tracker = GetBestTracker(args);//m_pFaceTrackers[id]; //todo: get best tracker
	if (tracker)
	{
		tracker->PaintEvent(message, args);
	}
}

void TrackerManager::TrackEvent(void* message, TrackingArgs args)
{
	//std::sort (m_pFaceTrackers.begin(), m_pFaceTrackers.end(), SortFaceTracking);
	if(m_CallBack)
	{
		void* viewArgs = static_cast<void*>(GetTrackingResults(message));
		m_CallBackArgs = viewArgs;
		(m_CallBack)(m_CallBackParam, m_CallBackArgs);
	}
};

bool SortFaceTracking (KinectFaceTracker* i,KinectFaceTracker* j) 
{ 
	return (i->GetFaceConfidence()>j->GetFaceConfidence()); 
}

KinectFaceTracker* TrackerManager::GetBestTracker(TrackingArgs args)
{	
	int id = args == NULL ? 0 : *(reinterpret_cast<int*>(args));
	m_pBestTracker = m_pFaceTrackers[id];
	return m_pBestTracker;
}




/*
* The "Face Tracker" helper class is generic. It will call back this function
* after a face has been successfully tracked. The code in the call back passes the parameters
* to the Egg Avatar, so it can be animated.
*/

TrackingResults*	TrackerManager::GetTrackingResults(TrackingArgs args)
{
	WeightResults();
	return &m_weightedResults;
}

void TrackerManager::WeightResults()
{
	m_weightedResults.eulerAngles[Pitch] = 0;
	m_weightedResults.eulerAngles[Yaw] = 0;
	m_weightedResults.eulerAngles[Roll] = 0;
	m_weightedResults.Translation[Xaxis] = 0;
	m_weightedResults.Translation[Yaxis] = 0;
	m_weightedResults.Translation[Zaxis] = 0;
	
	for (std::vector<KinectFaceTracker*>::iterator tracker = m_pFaceTrackers.begin(); tracker != m_pFaceTrackers.end(); ++tracker)
	{
		(*tracker)->UpdateAvatarPose();
		TrackingResults* results = (*tracker)->GetTrackingResults();
		m_weightedResults.eulerAngles[Pitch] += results->eulerAngles[Pitch];
		m_weightedResults.eulerAngles[Yaw] += results->eulerAngles[Yaw];
		m_weightedResults.eulerAngles[Roll] += results->eulerAngles[Roll];
		m_weightedResults.Translation[Xaxis] += results->Translation[Xaxis];
		m_weightedResults.Translation[Yaxis] += results->Translation[Yaxis];
		m_weightedResults.Translation[Zaxis] += results->Translation[Zaxis];
	}

	int numOfTrackers = m_pFaceTrackers.size();

	if (numOfTrackers > 0)
	{
		m_weightedResults.eulerAngles[Pitch] /=  numOfTrackers;
		m_weightedResults.eulerAngles[Yaw] /= numOfTrackers;
		m_weightedResults.eulerAngles[Roll] /= numOfTrackers;
		m_weightedResults.Translation[Xaxis] /= numOfTrackers;
		m_weightedResults.Translation[Yaxis] /= numOfTrackers;
		m_weightedResults.Translation[Zaxis] /= numOfTrackers;
	}
}

void TrackerManager::ParseCmdString(PWSTR lpCmdLine)
{
    const WCHAR KEY_DEPTH[]                                 = L"-Depth";
    const WCHAR KEY_COLOR[]                                 = L"-Color";
    const WCHAR KEY_NEAR_MODE[]                             = L"-NearMode";
    const WCHAR KEY_DEFAULT_DISTANCE_MODE[]                 = L"-DefaultDistanceMode";
    const WCHAR KEY_SEATED_SKELETON_MODE[]                  = L"-SeatedSkeleton";

    const WCHAR STR_NUI_IMAGE_TYPE_DEPTH[]                  = L"DEPTH";
    const WCHAR STR_NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX[] = L"PLAYERID";
    const WCHAR STR_NUI_IMAGE_TYPE_COLOR[]                  = L"RGB";
    const WCHAR STR_NUI_IMAGE_TYPE_COLOR_YUV[]              = L"YUV";

    const WCHAR STR_NUI_IMAGE_RESOLUTION_80x60[]            = L"80x60";
    const WCHAR STR_NUI_IMAGE_RESOLUTION_320x240[]          = L"320x240";
    const WCHAR STR_NUI_IMAGE_RESOLUTION_640x480[]          = L"640x480";
    const WCHAR STR_NUI_IMAGE_RESOLUTION_1280x960[]         = L"1280x960";

    enum TOKEN_ENUM
    {
        TOKEN_ERROR,
        TOKEN_DEPTH,
        TOKEN_COLOR,
        TOKEN_NEARMODE,
        TOKEN_DEFAULTDISTANCEMODE,
        TOKEN_SEATEDSKELETON
    }; 

    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(lpCmdLine, &argc);

    for(int i = 0; i < argc; i++)
    {
        NUI_IMAGE_TYPE* pType = NULL;
        NUI_IMAGE_RESOLUTION* pRes = NULL;

        TOKEN_ENUM tokenType = TOKEN_ERROR; 
        PWCHAR context = NULL;
        PWCHAR token = wcstok_s(argv[i], L":", &context);
        if(0 == wcsncmp(token, KEY_DEPTH, ARRAYSIZE(KEY_DEPTH)))
        {
            tokenType = TOKEN_DEPTH;
            pType = &m_depthType;
            pRes = &m_depthRes;
        }
        else if(0 == wcsncmp(token, KEY_COLOR, ARRAYSIZE(KEY_COLOR)))
        {
            tokenType = TOKEN_COLOR;
            pType = &m_colorType;
            pRes = &m_colorRes;
        }
        else if(0 == wcsncmp(token, KEY_NEAR_MODE, ARRAYSIZE(KEY_NEAR_MODE)))
        {
            tokenType = TOKEN_NEARMODE;
            m_bNearMode = TRUE;
        }
        else if(0 == wcsncmp(token, KEY_DEFAULT_DISTANCE_MODE, ARRAYSIZE(KEY_DEFAULT_DISTANCE_MODE)))
        {
            tokenType = TOKEN_DEFAULTDISTANCEMODE;
            m_bNearMode = FALSE;
        }
        else if(0 == wcsncmp(token, KEY_SEATED_SKELETON_MODE, ARRAYSIZE(KEY_SEATED_SKELETON_MODE)))
        {
            tokenType = TOKEN_SEATEDSKELETON;
            m_bSeatedSkeletonMode = TRUE;
        }

        if(tokenType == TOKEN_DEPTH || tokenType == TOKEN_COLOR)
        {
            _ASSERT(pType != NULL && pRes != NULL);

            while((token = wcstok_s(NULL, L":", &context)) != NULL)
            {
                if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_DEPTH, ARRAYSIZE(STR_NUI_IMAGE_TYPE_DEPTH)))
                {
                    *pType = NUI_IMAGE_TYPE_DEPTH;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, ARRAYSIZE(STR_NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX)))
                {
                    *pType = NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_COLOR, ARRAYSIZE(STR_NUI_IMAGE_TYPE_COLOR)))
                {
                    *pType = NUI_IMAGE_TYPE_COLOR;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_TYPE_COLOR_YUV, ARRAYSIZE(STR_NUI_IMAGE_TYPE_COLOR_YUV)))
                {
                    *pType = NUI_IMAGE_TYPE_COLOR_YUV;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_80x60, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_80x60)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_80x60;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_320x240, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_320x240)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_320x240;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_640x480, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_640x480)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_640x480;
                }
                else if(0 == wcsncmp(token, STR_NUI_IMAGE_RESOLUTION_1280x960, ARRAYSIZE(STR_NUI_IMAGE_RESOLUTION_1280x960)))
                {
                    *pRes = NUI_IMAGE_RESOLUTION_1280x960;
                }
            }
        }
    }

    if(argv) LocalFree(argv);
}