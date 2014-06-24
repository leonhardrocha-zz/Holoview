#include "TrackerManipulator.h"

namespace osgGA 
{
    void TrackerManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
    {
        _center = eye + rotation * osg::Vec3d( 0., 0., -_distance );
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

}