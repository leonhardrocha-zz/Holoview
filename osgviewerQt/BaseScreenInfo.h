#include "osgLib.h"
#ifndef _BASESCREENINFO_H
#define _BASESCREENINFO_H

class BaseScreenInfo
{
public:
    BaseScreenInfo() : 
    m_tvWidth(60 * 0.0254), // 60 inches
    m_tvHeight(36 * 0.0254), // 36 inches 
    m_tvDepth(1.5 * 0.0254), // 1.5 inches
    m_bezelWidth(2 * 0.0254), // 2 inches
    m_bezelHeight(2 * 0.0254), //2 inches
    m_screenWidth(56 * 0.0254), // 56 inches
    m_screenHeight(32 * 0.0254), // 32 inches
    m_screenDistance(1.0), // 1.0 m
    m_screenDepth(30.0), // 3.0 m
    m_tvElevation(0.75)
    {};

    double left, right, top, bottom, zNear, zFar;
    double zLeft, zRight;
    double m_tvElevation;
    double m_tvWidth;
    double m_tvHeight;
    double m_tvDepth;
    double m_bezelWidth;
    double m_bezelHeight;
    double m_screenWidth;
    double m_screenHeight;
    double m_screenDistance;
    double m_screenDepth;
};

#endif
