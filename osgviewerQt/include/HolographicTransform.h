#include "osgLib.h"
#ifndef _HOLOGRAPHIC_TRANSFORM_H
#define _HOLOGRAPHIC_TRANSFORM_H

using namespace osg;

class HolographicTransform : public PositionAttitudeTransform
{
public:
    virtual bool computeLocalToWorldMatrix(Matrix& matrix, NodeVisitor* nv) const
    {
        if (_referenceFrame==RELATIVE_RF)
        {
            matrix.preMultTranslate(_position);
            matrix.preMultRotate(_attitude);
            matrix.preMultScale(_scale);
            matrix.preMultTranslate(-_pivotPoint);
        }
        else // absolute
        {
            matrix.makeRotate(_attitude);
            matrix.postMultTranslate(_position);
            matrix.preMultScale(_scale);
            matrix.preMultTranslate(-_pivotPoint);
        }
        return true;
    };
    virtual bool computeWorldToLocalMatrix(Matrix& matrix, NodeVisitor* nv) const
    {
        if (_scale.x() == 0.0 || _scale.y() == 0.0 || _scale.z() == 0.0)
            return false;

        if (_referenceFrame==RELATIVE_RF)
        {
            matrix.postMultTranslate(-_position);
            matrix.postMultRotate(_attitude.inverse());
            matrix.postMultScale(Vec3d(1.0/_scale.x(), 1.0/_scale.y(), 1.0/_scale.z()));
            matrix.postMultTranslate(_pivotPoint);
        }
        else // absolute
        {
            matrix.makeRotate(_attitude.inverse());
            matrix.preMultTranslate(-_position);
            matrix.postMultScale(Vec3d(1.0/_scale.x(), 1.0/_scale.y(), 1.0/_scale.z()));
            matrix.postMultTranslate(_pivotPoint);
        }
        return true;
    };
}
#endif
