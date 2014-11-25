#include "osgLib.h"

#ifdef _MSC_VER
#pragma warning (disable : 4505 )
#endif

#ifndef _MATRIXEXTENSION_H
#define _MATRIXEXTENSION_H

namespace MatrixExtension
{

static osg::Matrix HolographicFrustum(double left, double right,
                         double bottom, double top,
                         double zNear, double zFar)
{
    osg::Matrix m;
    // note transpose of Matrix_implementation wr.t OpenGL documentation, since the OSG use post multiplication rather than pre.
    osg::Vec3 center((right+left)/(right-left), (top+bottom)/(top-bottom), (zFar+zNear)/(zFar-zNear));
    osg::Vec3 ratio(2.0*zNear/(right-left), 2.0*zNear/(top-bottom), 2.0*zNear/(zFar-zNear));
    double C = (fabs(zFar)>DBL_MAX) ? -1. : -center.z();
    double D = (fabs(zFar)>DBL_MAX) ? -2.*zNear : -ratio.z() * zFar;
    m.set(ratio.x(),             0.0,                  0.0,                    0.0,
          0.0,                   ratio.y(),            0.0,                    0.0,
          center.x(),            center.y(),           C,                     -1.0,
          0.0,                   0.0,                  D,                      0.0);
    return m;
}


static osg::Matrix getInverseRotation(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up)
{
    osg::Vec3 f(center-eye);
    f.normalize();
    osg::Vec3 s(f^up);
    s.normalize();
    osg::Vec3 u(s^f);
    u.normalize();
    return osg::Matrix(
        s[0],     u[0],     -f[0],     0.0,
        s[1],     u[1],     -f[1],     0.0,
        s[2],     u[2],     -f[2],     0.0,
        0.0,     0.0,     0.0,      1.0);

}

static osg::Matrix getShear(const osg::Vec3& sh)
{
    
    return osg::Matrix(
        1.0,    0.0,     0.0,      0.0,
        0.0,    1.0,     0.0,      0.0,
        sh.x(), sh.y(),  sh.z(),      0.0,
        0.0,    0.0,     0.0,      1.0);

}

static osg::Matrix getRotation(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up)
{
    osg::Vec3 f(center-eye);
    f.normalize();
    osg::Vec3 s(f^up);
    s.normalize();
    osg::Vec3 u(s^f);
    u.normalize();
    return osg::Matrix(
        s[0],     s[1],      s[2],     0.0,
        u[0],     u[1],      u[2],     0.0,
        f[0],     f[1],      f[2],     0.0,
        0.0,     0.0,     0.0,      1.0);

}

static osg::Matrix getViewRotation(const osg::Vec3& eye, const osg::Vec3& center, const osg::Vec3& up)
{
    osg::Vec3 f(center-eye);
    f.normalize();
    osg::Vec3 s(f^up);
    s.normalize();
    osg::Vec3 u(s^f);
    u.normalize();
    return osg::Matrix(
        s[0],     s[1],      s[2],     0.0,
        u[0],     u[1],      u[2],     0.0,
       -f[0],    -f[1],     -f[2],     0.0,
        0.0,     0.0,     0.0,      1.0);

}


}
#endif