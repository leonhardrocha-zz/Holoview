//------------------------------------------------------------------------------
// <copyright file="SingleFace.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------
#pragma once

#include <FaceTrackLib.h>
#include "resource.h"
//#include "EggAvatar.h"
#include "MultiFTHelper.h"

class KinectTracker;

class SingleFace
{
	friend class KinectTracker;
public:
    SingleFace::SingleFace()
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
	~SingleFace()	{ UninitInstance();	};
	

    int							Run(HINSTANCE hInst, PWSTR lpCmdLine, int nCmdShow);
	HWND						GetHWnd() { return m_hWnd;};
	HINSTANCE					GetInstance() { return m_hInst; };
	BOOL						InitInstance();
	void						InitArgs(int argc, char **argv);
	BOOL						InitInstanceInHostWindow();

	float m_Pitch, m_Yaw, m_Roll;

protected:

	PWSTR m_lpCmdLine;
	int m_nCmdShow;

	HWND						createWindow(HWND parent, HINSTANCE instance);
	BOOL                        InitInstance(HINSTANCE hInst, PWSTR lpCmdLine, int nCmdShow);

    void                        ParseCmdString(PWSTR lpCmdLine);
    void                        UninitInstance();
    ATOM                        RegisterClass(PCWSTR szWindowClass);
    static LRESULT CALLBACK     WndProcStatic(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK            WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK     About(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    BOOL                        PaintWindow(HDC hdc, HWND hWnd);
    BOOL                        ShowVideo(HDC hdc, int width, int height, int originX, int originY);
    BOOL                        ShowEggAvatar(HDC hdc, int width, int height, int originX, int originY);
    static void                 FTHelperCallingBack(LPVOID lpParam);
    static int const            MaxLoadStringChars = 100;

    HINSTANCE                   m_hInst;
    HWND                        m_hWnd;
    HACCEL                      m_hAccelTable;
    //FTHelper                    m_DualFTHelper;
	//DualFTHelper                m_DualFTHelper;
	MultiFTHelper               m_MultiFTHelper;
    IFTImage*                   m_pImageBuffer;
    IFTImage*                   m_pVideoBuffer;
    NUI_IMAGE_TYPE              m_depthType;
    NUI_IMAGE_TYPE              m_colorType;
    NUI_IMAGE_RESOLUTION        m_depthRes;
    NUI_IMAGE_RESOLUTION        m_colorRes;
    BOOL                        m_bNearMode;
    BOOL                        m_bSeatedSkeletonMode;
	EggAvatar*                  m_pEggAvatar;
	TrackerPair*				m_pSensorTrackerPair;	
};

