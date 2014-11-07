#include "stdafx.h"
#ifndef _TRACKERCONFIG_H
#define _TRACKERCONFIG_H
#include <FaceTrackLib.h>
#include <NuiApi.h>
class TrackerConfig
{
public: 
    TrackerConfig() :
        DrawMask(TRUE),
        DepthType(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX),
        DepthResolution(NUI_IMAGE_RESOLUTION_640x480),
        IsNearMode(TRUE),
        IsFallbackToDefault(TRUE),
        IsSeatedSkeletonMode(TRUE),
        ColorType(NUI_IMAGE_TYPE_COLOR),    
        ColorResolution(NUI_IMAGE_RESOLUTION_1280x960)
    {}
    BOOL                 DrawMask;
    NUI_IMAGE_TYPE       DepthType;
    NUI_IMAGE_RESOLUTION DepthResolution;
    BOOL                 IsNearMode;
    BOOL                 IsFallbackToDefault;
    BOOL                 IsSeatedSkeletonMode;
    NUI_IMAGE_TYPE       ColorType;
    NUI_IMAGE_RESOLUTION ColorResolution;
};
#endif