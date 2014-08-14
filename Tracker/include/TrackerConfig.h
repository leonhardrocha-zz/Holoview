#include "stdafx.h"
#ifndef _TRACKERCONFIG_H
#define _TRACKERCONFIG_H
#include "KinectSensor.h"
class TrackerConfig
{
public: 
    TrackerConfig() :
        m_DrawMask(TRUE),
        m_depthType(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX),
        m_depthRes(NUI_IMAGE_RESOLUTION_640x480),
        m_bNearMode(TRUE),
        m_bFallbackToDefault(TRUE),
        m_bSeatedSkeletonMode(TRUE),
        m_colorType(NUI_IMAGE_TYPE_COLOR),    
        m_colorRes(NUI_IMAGE_RESOLUTION_1280x960)
    {}
    BOOL                 m_DrawMask;
    NUI_IMAGE_TYPE       m_depthType;
    NUI_IMAGE_RESOLUTION m_depthRes;
    BOOL                 m_bNearMode;
    BOOL                 m_bFallbackToDefault;
    BOOL                 m_bSeatedSkeletonMode;
    NUI_IMAGE_TYPE       m_colorType;
    NUI_IMAGE_RESOLUTION m_colorRes;
};
#endif