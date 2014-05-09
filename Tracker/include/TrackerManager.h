//------------------------------------------------------------------------------
// <copyright file="TrackerManager.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#pragma once

#include <FaceTrackLib.h>
#include "resource.h"
#include "EggAvatar.h"
#include "MultiFTHelper.h"
#include "KinectFaceTracker.h"
#include "Callable.h"
#include <vector>


class KinectTracker;

class TrackerManager : public Callable
{
	friend class KinectTracker;
public:
    TrackerManager::TrackerManager()
		: m_hInst(NULL)
        , m_hWnd(NULL)
		, m_lpCmdLine(L"")
        , m_hAccelTable(NULL)
        , m_pImageBuffer(NULL)
        , m_pVideoBuffer(NULL)
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
	void						PaintEvent(void *message);
	void						TrackEvent(void *message) {};
	HWND						GetHWnd() { return m_hWnd;};
	HINSTANCE					GetInstance() { return m_hInst; };
	void						InitArgs(int argc, char **argv);
	BOOL						InitInstanceInHostWindow();
	float						scale;
    float						rotationXYZ[3];
    float						translationXYZ[3];

protected:


	KinectFaceTracker*			m_pBestTracker;
	KinectFaceTracker*			GetBestTracker();
	PWSTR m_lpCmdLine;
	int m_nCmdShow;
	std::vector<KinectFaceTracker*>	m_pFaceTrackers;
	std::vector<HANDLE>			m_FaceTrackingThreads;
	bool						IsTracking();
    void                        ParseCmdString(PWSTR lpCmdLine);
    void                        UninitInstance();
    static LRESULT CALLBACK     WndProcStatic(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK            WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    BOOL                        PaintWindow(HDC hdc, HWND hWnd);
    BOOL                        ShowVideo(HDC hdc, int width, int height, int originX, int originY);
    BOOL                        ShowEggAvatar(HDC hdc, int width, int height, int originX, int originY);
    static void                 FTHelperCallingBack(void* lpParam);
    static int const            MaxLoadStringChars = 100;
    HINSTANCE                   m_hInst;
    HWND                        m_hWnd;
    HACCEL                      m_hAccelTable;
    IFTImage*                   m_pImageBuffer;
    IFTImage*                   m_pVideoBuffer;
    NUI_IMAGE_TYPE              m_depthType;
    NUI_IMAGE_TYPE              m_colorType;
    NUI_IMAGE_RESOLUTION        m_depthRes;
    NUI_IMAGE_RESOLUTION        m_colorRes;
    BOOL                        m_bNearMode;
    BOOL                        m_bSeatedSkeletonMode;
};

