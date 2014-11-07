#ifdef _MSC_VER
#pragma once
#endif

#include "stdafx.h"
#include "KinectTracker.h"
#include "TrackerException.h"

void KinectTracker::PaintEvent(IArgs* args)
{
    const MSG* msg = reinterpret_cast<const MSG*>(args->Get(WindowHandlerArg));
    if (msg != NULL)
    {
        PAINTSTRUCT ps;
        HDC hdc= BeginPaint(msg->hwnd, &ps);
        // Draw the avatar window and the video window
        GetBestTracker()->PaintWindow(hdc, msg->hwnd);
        EndPaint(msg->hwnd, &ps);
    }
}