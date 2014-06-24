#pragma once

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/config/PanoramicSphericalDisplay>
#include <osg/Camera>
#include <osgGA/CameraManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/StandardManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>

namespace osgGA {

class TrackerManipulator : public TrackballManipulator
{
    public:
        TrackerManipulator(int flags = DEFAULT_SETTINGS) : osgGA::TrackballManipulator(flags), _obj_distance(0.0) {};
        TrackerManipulator( const TrackballManipulator& tm,
            const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY ) : TrackballManipulator(tm, copyOp), _obj_distance(0.0) {};
        ~TrackerManipulator() {};

        META_Object( osgGA, TrackerManipulator );

        virtual void setTransformation(const osg::Vec3d& eye, const osg::Quat& rotation );
        virtual void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up );
        virtual void translate( const osg::Vec3d& translation );
        virtual void setObjectDistance(const double distance ) { _obj_distance = distance; };
    protected:
        double _obj_distance;
};

}
