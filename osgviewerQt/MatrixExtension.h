#include "stdafx.h"
#ifndef _MATRIXEXTENSION_H
#define _MATRIXEXTENSION_H
static osg::Matrix HolographicFrustum(double left, double right,
                         double bottom, double top,
                         double zNear, double zFar)
{
    osg::Matrix m;
    // note transpose of Matrix_implementation wr.t OpenGL documentation, since the OSG use post multiplication rather than pre.
    double A = (right+left)/(right-left);
    double B = (top+bottom)/(top-bottom);
    double C = (fabs(zFar)>DBL_MAX) ? -1. : -(zFar-zNear)/(zFar+zNear);
    double D = (fabs(zFar)>DBL_MAX) ?  2.*zNear : 2.0*zFar*zNear/(zFar+zNear);
    m.set(2.0*zNear/(right-left), 0.0,                    0.0,  0.0,
          0.0,                    2.0*zNear/(top-bottom), 0.0,  0.0,
          A,                      B,                      C,    1.0,
          0.0,                    0.0,                    D,    0.0);
    return m;
}
#endif