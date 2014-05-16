//------------------------------------------------------------------------------
// <copyright file="TrackerManager.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#pragma once

#include <FaceTrackLib.h>
#include "resource.h"
#include "EggAvatar.h"
#include "KinectFaceTracker.h"
#include "Callable.h"
#include "ITracker.h"
#include <vector>


class KinectTracker;

class TrackerManager : public ITracker
{
	friend class KinectTracker;
public:
    TrackerManager::TrackerManager()
		: m_hInst(NULL)
		, m_lpCmdLine(L"")
        , m_hAccelTable(NULL)
        , m_depthType(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX)
        , m_colorType(NUI_IMAGE_TYPE_COLOR)
        , m_depthRes(NUI_IMAGE_RESOLUTION_320x240)
        , m_colorRes(NUI_IMAGE_RESOLUTION_640x480)
        , m_bNearMode(TRUE)
        , m_bSeatedSkeletonMode(TRUE)
{

}	
	~TrackerManager()	{ UninitInstance();	};
	

	bool						Init();
	bool						Start();
	HINSTANCE					GetInstance() { return m_hInst; };
	void						InitArgs(int argc, char **argv);
	BOOL						InitInstanceInHostWindow();
	TrackingResults				GetTrackingResults(TrackingArgs args=NULL);

protected:

	virtual		void			PaintEvent(void *message, TrackingArgs args=NULL);
	virtual		void			TrackEvent(void *message, TrackingArgs args=NULL);

	std::vector<KinectFaceTracker*>	m_pFaceTrackers;
	std::vector<HANDLE>			m_FaceTrackingThreads;

	float						scale;
    float						rotationXYZ[3];
    float						translationXYZ[3];
	KinectFaceTracker*			m_pBestTracker;
	PWSTR						m_lpCmdLine;
	int							m_nCmdShow;
    void                        ParseCmdString(PWSTR lpCmdLine);
    void                        UninitInstance();
	bool						IsTracking();
	virtual		KinectFaceTracker*			GetBestTracker(TrackingArgs args=0);
    static int const            MaxLoadStringChars = 100;

    HINSTANCE                   m_hInst;
    /*HWND                        m_hWnd;*/
    HACCEL                      m_hAccelTable;
    NUI_IMAGE_TYPE              m_depthType;
    NUI_IMAGE_TYPE              m_colorType;
    NUI_IMAGE_RESOLUTION        m_depthRes;
    NUI_IMAGE_RESOLUTION        m_colorRes;
    BOOL                        m_bNearMode;
    BOOL                        m_bSeatedSkeletonMode;
};

class MultiTrackerManager : public TrackerManager
{
protected:
	virtual		BOOL            PaintWindow(KinectFaceTracker *tracker, HDC hdc, HWND hWnd); 

};