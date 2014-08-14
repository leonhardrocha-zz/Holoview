#pragma once
#include "stdafx.h"
#include "TrackerManager.h"


// Draw the camera video with the mask superimposed.
BOOL MultiTrackerManager::PaintWindow(KinectFaceTracker *tracker, HDC hdc, HWND hWnd)
{        
    static int errCount = 0;
    BOOL ret = FALSE;
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    //int halfWidth = width/2;
    // Show the video on the right of the window
    if (tracker->IsKinectPresent())
    {
        errCount += !tracker->ShowVideo(hdc, width, height, 0, 0);
        // Draw the egg avatar on the left of the window
        /*errCount += !ShowEggAvatar(tracker, hdc, halfWidth, height, 0, 0);*/
    }
    
    return ret;
}