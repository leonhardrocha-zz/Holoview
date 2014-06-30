#include "stdafx.h"
#ifndef _TRACKERMANIPULATOR_H
#define _TRACKERMANIPULATOR_H



namespace osgGA {

    class TrackerManipulator : public TrackballManipulator
    {
        public:
            TrackerManipulator(int flags = DEFAULT_SETTINGS) : osgGA::TrackballManipulator(flags) {};
            TrackerManipulator( const TrackballManipulator& tm,
                const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY ) : TrackballManipulator(tm, copyOp) {};
            ~TrackerManipulator() {};

            META_Object( osgGA, TrackerManipulator );

            virtual void setTransformation(const osg::Vec3d& eye, const osg::Quat& rotation );
            virtual void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up );
        protected:
    };

}
#endif