#ifdef _MSC_VER
#pragma once
#endif

#include "stdafx.h"
#include "KinectTracker.h"
#include "KinectFaceTracker.h"
#include "TrackerException.h"

KinectTracker::KinectTracker(ITracker* parent, bool initialize, bool start) : TrackerManager(parent, initialize, start)
{
    if(initialize)
    {
        IsInitialized = Init();
    }
    if (IsInitialized && start)
    {
        IsRunning = Start();
    }
}

KinectTracker::~KinectTracker()
{
    if(m_hWnd)
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

bool KinectTracker::GetPosition(double& x, double& y, double& z)
{
    if(!m_pFaceTracker)
    {
        return false;
    }

    EggAvatar* avatar = GetEggAvatar();
    IPose* position = avatar->GetPosition();
    x = position->Get(Position::XAxis);
    y = position->Get(Position::YAxis);
    z = position->Get(Position::ZAxis);

    return true;
}

bool KinectTracker::Init()
{
    m_pImageBuffer = FTCreateImage();
    m_pVideoBuffer = FTCreateImage();
    m_pImageBuffer->Allocate(1, 1, FTIMAGEFORMAT_UINT8_B8G8R8A8); //dummy alloc
    m_pVideoBuffer->Allocate(1, 1, FTIMAGEFORMAT_UINT8_B8G8R8A8); //dummy alloc
    return true;
}

bool KinectTracker::Start()
{
    SetTrackerUpdateCallback(Callback(KinectTracker::UpdateVideoCallback, this));
    IsShowingVideo = false;
    FitToWindow = true;
    return true;
}

void KinectTracker::UpdateVideoCallback(void* instance)
{
    KinectTracker *pThis = static_cast<KinectTracker*>(instance);
    if (pThis && pThis->IsShowingVideo)
    {
        RECT rect;
        GetClientRect(pThis->m_hWnd, &rect);
        InvalidateRect(pThis->m_hWnd, &rect, FALSE);
    }
}

bool KinectTracker::AddVideoUpdateCallback(const Callback& callback)
{
    VideoCallbacks.push_back(callback);

    return true;
}


bool KinectTracker::SetWindowHandler(void* handler)
{
    MSG* msg = reinterpret_cast<MSG*>(handler);
    if (msg != NULL && msg->hwnd !=NULL)
    {
        m_hWnd = msg->hwnd;
        return true;
    }
    return false;
}

void KinectTracker::TrackEvent(void* message)
{
    IFTResult* pResult = static_cast<IFTResult*>(message);
    if (pResult == NULL)
    {
        return;
    }
    if (IsShowingVideo)
    {
        RECT faceRect;
        pResult->GetFaceRect(&faceRect);
        SetCenterOfImage(faceRect);
        UpdateVideo();
    }
}

void KinectTracker::PaintEvent(void* message)
{
    IsShowingVideo = SetWindowHandler(message);
    if (IsShowingVideo)
    {
        UpdateWindow();
    }
}


bool KinectTracker::UpdateWindow()
{
    if(!IsRunning || !IsShowingVideo)
    {
        return false;
    }

    static int errCount = 0;
    RECT rect;
    GetClientRect(m_hWnd, &rect);
    PAINTSTRUCT ps;
    m_hdc= BeginPaint(m_hWnd, &ps);
    // Draw the avatar window and the video window
    bool ret = PaintWindow();
    EndPaint(m_hWnd, &ps);

    return ret;
}

void KinectTracker::UpdateVideo()
{
    if (!IsRunning || !IsShowingVideo) 
    {
        return;
    }
    //BOOL ret = FALSE;
    //RECT rect;
    //GetClientRect(m_hWnd, &rect);
    InvalidateRect(m_hWnd, NULL, FALSE);
    if(UpdateWindow())
    {
        Sleep(16);
    }
}

// Draw the egg head and the camera video with the mask superimposed.
bool KinectTracker::PaintWindow()
{    
    if(!IsShowingVideo)
    {
        return false;
    }

    static int errCount = 0;

    RECT rect;
    GetClientRect(m_hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    // Show the video on the right of the window
    int halfWidth = IsShowingAvatar ? width/2 : 0;
    errCount += !ShowVideo(width - halfWidth, height, halfWidth, 0);
    if (IsShowingAvatar)
    {
        // Draw the egg avatar on the left of the window
        errCount += !ShowEggAvatar(halfWidth, height, 0, 0);
    }

    return true;
}


// Drawing the video window
bool KinectTracker::ShowVideo(int width, int height, int originX, int originY)
{
    if(!m_hWnd)
    {
        return false;
    }

    if(!m_pFaceTrackers.empty())
    {
        m_pFaceTracker = GetBestTracker();
    } 
    else
    {
        return false;
    }

    // Now, copy a fraction of the camera image into the screen.
    IFTImage* colorImage = m_pFaceTracker->GetColorImage();
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

                if(m_pFaceTracker->LastTrackSucceeded)
                {
                    m_pFaceTracker->GetFaceModel(m_pVideoBuffer);
                }

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
                
                if(m_hdc)
                {
                    int const bmpPixSize = m_pVideoBuffer->GetBytesPerPixel();
                    SetStretchBltMode(m_hdc, HALFTONE);
                    BITMAPINFO bmi = {sizeof(BITMAPINFO), iWidth, iHeight, 1, static_cast<WORD>(bmpPixSize * CHAR_BIT), BI_RGB, m_pVideoBuffer->GetStride() * iHeight, 5000, 5000, 0, 0};
                    if (0 == StretchDIBits(m_hdc, originX, originY, width, height,
                        iLeft, iBottom, iRight-iLeft, iTop-iBottom, m_pVideoBuffer->GetBuffer(), &bmi, DIB_RGB_COLORS, SRCCOPY))
                    {
                        return false;
                    } 
                }
            }
        }

        
        return true;
    }
    return false;
}

// Drawing code
bool KinectTracker::ShowEggAvatar(int width, int height, int originX, int originY)
{
    static int errCount = 0;
    bool ret = false;

    if (m_pImageBuffer && SUCCEEDED(m_pImageBuffer->Allocate(width, height, FTIMAGEFORMAT_UINT8_B8G8R8A8)))
    {
        memset(m_pImageBuffer->GetBuffer(), 0, m_pImageBuffer->GetStride() * height); // clear to black
        m_eggAvatar.SetScaleAndTranslationToWindow(height, width);
        m_eggAvatar.DrawImage(static_cast<PVOID>(m_pImageBuffer));
        BITMAPINFO bmi = {sizeof(BITMAPINFO), width, height, 1, static_cast<WORD>(m_pImageBuffer->GetBytesPerPixel() * CHAR_BIT), BI_RGB, m_pImageBuffer->GetStride() * height, 5000, 5000, 0, 0};
        errCount += (0 == StretchDIBits(m_hdc, 0, 0, width, height, 0, 0, width, height, m_pImageBuffer->GetBuffer(), &bmi, DIB_RGB_COLORS, SRCCOPY));
        ret = true;
    }

    return ret;
}



// We compute here the nominal "center of attention" that is used when zooming the presented image.
void KinectTracker::SetCenterOfImage(const RECT& faceRect)
{
    if(m_pFaceTracker)
    {
        IFTImage* colorImage = m_pFaceTracker->GetColorImage();
        double centerX = colorImage->GetWidth()/2.0;
        double centerY = colorImage->GetHeight()/2.0;
        if (m_pFaceTracker->LastTrackSucceeded)
        {
            centerX = (faceRect.left+faceRect.right)/2.0;
            centerY = (faceRect.top+faceRect.bottom)/2.0;
        }
        m_XCenterFace += 0.02 *(centerX-m_XCenterFace);
        m_YCenterFace += 0.02 *(centerY-m_YCenterFace);
    }
}

