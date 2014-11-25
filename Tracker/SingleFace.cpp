//------------------------------------------------------------------------------
// <copyright file="TrackerManager.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Defines the entry point for the application.
//
#include "stdafx.h"
#include "TrackerManager.h"

HWND	TrackerManager::createWindow(HWND parent, HINSTANCE instance)
{
	m_hInst = instance;

	WCHAR szWindowClass[] = L"TrackerManager";            // the main window class name
	//LoadString(m_hInst, IDC_TrackerManager, szWindowClass, ARRAYSIZE(szWindowClass));		

	static ATOM windowClass = RegisterClass(szWindowClass);

	m_hWnd = CreateWindow((TCHAR*)windowClass, 0, WS_CHILD|WS_CLIPSIBLINGS|WS_TABSTOP, 
						CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, parent, NULL, instance, NULL);

	return m_hWnd;
}
		

HRESULT TrackerManager::Start()
{      
	int numSensors = 0;	
    NuiGetSensorCount(&numSensors);

	for (int i =0; i < numSensors; i++)
	{
		KinectFaceTracker tracker;
		if (tracker.InitTracker(FTHelperCallingBack, this))
		{
			m_FaceTrackers.push_back(tracker);
			auto iterator = m_FaceTrackers.end();
			HANDLE threadId = CreateThread(NULL, 0, KinectFaceTracker::FaceTrackingStaticThread, (PVOID)&(*iterator), 0, 0);
			m_FaceTrackingThreads.push_back(threadId);		
		}
	}
			  
    return S_OK;
}



int TrackerManager::Run(HINSTANCE hInst, PWSTR lpCmdLine, int nCmdShow)
{
    MSG msg = {static_cast<HWND>(0), static_cast<UINT>(0), static_cast<WPARAM>(-1)};
    if (InitInstance(hInst, lpCmdLine, nCmdShow))
    {
        // Main message loop:
        while (GetMessage(&msg, NULL, 0, 0))
        {
            if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    UninitInstance();

    return (int)msg.wParam;
}

// In this function, we save the instance handle, then create and display the main program window.
BOOL TrackerManager::InitInstance(HINSTANCE hInstance, PWSTR lpCmdLine, int nCmdShow)
{
    m_hInst = hInstance; // Store instance handle in our global variable

    ParseCmdString(lpCmdLine);

    WCHAR szTitle[MaxLoadStringChars];                  // The title bar text
    LoadString(m_hInst, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle));

    static const PCWSTR RES_MAP[] = { L"80x60", L"320x240", L"640x480", L"1280x960" };
    static const PCWSTR IMG_MAP[] = { L"PLAYERID", L"RGB", L"YUV", L"YUV_RAW", L"DEPTH" };

    // Add mode params in title
    WCHAR szTitleComplete[MAX_PATH];
    swprintf_s(szTitleComplete, L"%s -- Depth:%s:%s Color:%s:%s NearMode:%s, SeatedSkeleton:%s", szTitle,
        IMG_MAP[m_depthType], (m_depthRes < 0)? L"ERROR": RES_MAP[m_depthRes], IMG_MAP[m_colorType], (m_colorRes < 0)? L"ERROR": RES_MAP[m_colorRes], m_bNearMode? L"ON": L"OFF",
        m_bSeatedSkeletonMode?L"ON": L"OFF");

    WCHAR szWindowClass[MaxLoadStringChars];            // the main window class name
    LoadString(m_hInst, IDC_SINGLEFACE, szWindowClass, ARRAYSIZE(szWindowClass));

    RegisterClass(szWindowClass);

    m_hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TrackerManager));

    m_pImageBuffer = FTCreateImage();
    m_pVideoBuffer = FTCreateImage();

    m_hWnd = CreateWindow(szWindowClass, szTitleComplete, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, m_hInst, this);
    if (!m_hWnd)
    {
        return FALSE;
    }

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

	return SUCCEEDED(m_MultiFTHelper.Init(	m_depthType,
											m_depthRes,
											m_bNearMode,
											TRUE,
											m_bSeatedSkeletonMode,
											m_colorType,
											m_colorRes,
											m_hWnd,
											FTHelperCallingBack, 
											this ));
}
void TrackerManager::InitArgs(int argc, char **argv )
{
	auto requiredSize = mbstowcs( NULL, argv[0], 0) + 1;
	m_lpCmdLine = new wchar_t[requiredSize];
	mbstowcs(m_lpCmdLine, argv[0], requiredSize);
	m_nCmdShow = argc;
	ParseCmdString(m_lpCmdLine);
}

BOOL TrackerManager::InitInstance()
{
	ParseCmdString(m_lpCmdLine);
	m_pImageBuffer = FTCreateImage();
    m_pVideoBuffer = FTCreateImage();	
	return SUCCEEDED(m_MultiFTHelper.Init(	m_depthType,
											m_depthRes,
											m_bNearMode,
											TRUE,
											m_bSeatedSkeletonMode,
											m_colorType,
											m_colorRes,
											m_hWnd,
											FTHelperCallingBack, 
											this ));
}


void TrackerManager::UninitInstance()
{
    // Clean up the memory allocated for Face Tracking and rendering.
	m_MultiFTHelper.Stop();

    if (m_hAccelTable)
    {
        DestroyAcceleratorTable(m_hAccelTable);
        m_hAccelTable = NULL;
    }

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
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
}


// Register the window class.
ATOM TrackerManager::RegisterClass(PCWSTR szWindowClass)
{
    WNDCLASSEX wcex = {0};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = &TrackerManager::WndProcStatic;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = m_hInst;
    wcex.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_TrackerManager));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_TrackerManager);
    wcex.lpszClassName  = szWindowClass;

    return RegisterClassEx(&wcex);
}

LRESULT CALLBACK TrackerManager::WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{    
	static TrackerManager* pThis = NULL; // cheating, but since there is just one window now, it will suffice.
    if (WM_CREATE == message)
    {
        pThis = reinterpret_cast<TrackerManager*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
    }
    return pThis ? pThis->WndProc(hWnd, message, wParam, lParam) : DefWindowProc(hWnd, message, wParam, lParam);
}


//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_KEYUP    - Exit in response to ESC key
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK TrackerManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(m_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_KEYUP:
        if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // Draw the avatar window and the video window
        PaintWindow(hdc, hWnd);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK TrackerManager::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Drawing the video window
BOOL TrackerManager::ShowVideo(HDC hdc, int width, int height, int originX, int originY)
{
    // Now, copy a fraction of the camera image into the screen.
	KinectFaceTracker& tracker = GetBestTracker();
	IFTImage* colorImage = tracker.GetColorImage();
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
                    iLeft = (int)max(0, m_MultiFTHelper.GetXCenterFace() - wx / 2);
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
                    iTop = (int)max(0, m_MultiFTHelper.GetYCenterFace() - hy / 2);
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
BOOL TrackerManager::ShowEggAvatar(HDC hdc, int width, int height, int originX, int originY)
{
    static int errCount = 0;
    BOOL ret = FALSE;
	
	KinectFaceTracker& tracker = GetBestTracker();

	IAvatar* pEggAvatar = tracker.GetAvatar();
	
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

bool BestFaceTracking (KinectFaceTracker& i,KinectFaceTracker& j) 
{ 
	return (i.GetFaceConfidence()>j.GetFaceConfidence()); 
}

KinectFaceTracker& TrackerManager::GetBestTracker()
{
	std::sort (m_FaceTrackers.begin(), m_FaceTrackers.end(), BestFaceTracking);
	return *m_FaceTrackers.begin();
}

// Draw the egg head and the camera video with the mask superimposed.
BOOL TrackerManager::PaintWindow(HDC hdc, HWND hWnd)
{
    static int errCount = 0;
    BOOL ret = FALSE;
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int halfWidth = width/2;
    // Show the video on the right of the window
	KinectFaceTracker& tracker = GetBestTracker();

	if (tracker.IsKinectPresent())
	{
		tracker.SetWindow(hWnd);

		float pitch = tracker.GetPitch();

		errCount += !ShowVideo(hdc, width - halfWidth, height, halfWidth, 0);

		// Draw the egg avatar on the left of the window
		errCount += !ShowEggAvatar(hdc, halfWidth, height, 0, 0);
	}

    return ret;
}

/*
* The "Face Tracker" helper class is generic. It will call back this function
* after a face has been successfully tracked. The code in the call back passes the parameters
* to the Egg Avatar, so it can be animated.
*/
void TrackerManager::FTHelperCallingBack(PVOID pVoid)
{
    TrackerManager* pApp = reinterpret_cast<TrackerManager*>(pVoid);
    if (pApp)
    {
		KinectFaceTracker& bestTracker = pApp->GetBestTracker();
        IFTResult* pResult = bestTracker.GetResult();
        if (pResult && SUCCEEDED(pResult->GetStatus()))
        {
            FLOAT* pAU = NULL;
            UINT numAU;
            pResult->GetAUCoefficients(&pAU, &numAU);
            FLOAT scale;
            FLOAT rotationXYZ[3];
            FLOAT translationXYZ[3];
            pResult->Get3DPose(&scale, rotationXYZ, translationXYZ);
			IAvatar* pEggAvatar = bestTracker.GetAvatar();
			if (pEggAvatar)
			{
				pEggAvatar->SetTranslations(translationXYZ[0], translationXYZ[1], translationXYZ[2]);
				pEggAvatar->SetRotations(rotationXYZ[0], rotationXYZ[1], rotationXYZ[2]);
			}
        }
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



