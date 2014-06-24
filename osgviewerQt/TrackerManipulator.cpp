#include "TrackerManipulator.h"

namespace osgGA 
{
    void TrackerManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
    {
        _center = eye + rotation * osg::Vec3d( 0., 0., -(_distance - _obj_distance) ) + _rotation * osg::Vec3d( 0., 0., -_obj_distance );
        _rotation = rotation;

        // fix current rotation
        if( getVerticalAxisFixed() )
        {
            fixVerticalAxis( _center, _rotation, true );
        }
    }

    void TrackerManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
    {
        ::osgGA::TrackballManipulator::setTransformation(eye, center, up);
    }

    void TrackerManipulator::translate( const osg::Vec3d& translation )
    {
        _center = _center - translation;
    }

}