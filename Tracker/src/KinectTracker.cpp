#ifdef _MSC_VER
#pragma once
#endif

#include "stdafx.h"
#include "KinectTracker.h"
#include "KinectFaceTracker.h"
#include "TrackerException.h"

void KinectTracker::PaintEvent(IArgs* args)
{
    if (args && args->Exists(WindowHandlerArg))
    {
        MSG* msg = reinterpret_cast<MSG*>(args->Get(WindowHandlerArg));
        if (msg != NULL && msg->hwnd !=NULL)
        {
            PAINTSTRUCT ps;
            HDC hdc= BeginPaint(msg->hwnd, &ps);
            KinectFaceTracker* faceTracker = GetBestTracker(args);
            faceTracker->SetWindow(msg->hwnd);
            // Draw the avatar window and the video window
            faceTracker->PaintWindow(hdc, msg->hwnd);
            EndPaint(msg->hwnd, &ps);
        }
    }
}
