#include "osgLib.h"
#ifndef _DISPLAYINFO_H
#define _DISPLAYINFO_H

class DisplayInfo
{
public:
    static double DisplayInfo::inInches(double inches) { return inches * 0.0254; };
    static double DisplayInfo::inCentimeters(double cm) { return cm * 0.01; };

    DisplayInfo(double elevation = inInches(27), double width = inInches(60), double height = inInches(36), double depth = inInches(1.5)) : 
    Elevation(elevation),
    Width(width),
    Height(height), 
    Depth(depth),
    BezelWidth(inInches(2)),
    BezelHeight(inInches(2))
    {};

    double Width;
    double Height;
    double Depth;
    double BezelWidth;
    double BezelHeight;
    double Elevation;
};
#endif
