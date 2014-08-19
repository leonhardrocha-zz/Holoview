#include "osgLib.h"
#ifndef _OBLIQUECAMERA_H
#define _OBLIQUECAMERA_H

class ObliqueCamera : public osg::Camera
{
public:
    osg::Matrix getRotationMatrix() { return m_rotation; }
    void setRotationMatrix(osg::Matrix rotation) { m_rotation = rotation; }
protected:
    osg::Matrix m_rotation;
};

#endif