#include "osgLib.h"
#ifndef _TRACKERMANIPULATOR_H
#define _TRACKERMANIPULATOR_H

#include "IArgs.h"

namespace osgGA {

    class TrackerManipulator : public FirstPersonManipulator
    {
        typedef FirstPersonManipulator inherited;

        public:

            TrackerManipulator( int flags = StandardManipulator::DEFAULT_SETTINGS );
            TrackerManipulator( const TrackerManipulator& fpm,
                                    const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY );

            META_Object( osgGA, TrackerManipulator );

            virtual void setByMatrix( const osg::Matrixd& matrix );
            virtual void setByInverseMatrix( const osg::Matrixd& matrix );
            virtual osg::Matrixd getMatrix() const;
            virtual osg::Matrixd getInverseMatrix() const;

            virtual void setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation );
            virtual void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up );
            virtual void getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const;
            virtual void getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const;

            virtual void setTrackingResults( IArgs* results, osg::Vec3 center);

            virtual void setVelocity( const double& velocity );
            inline double getVelocity() const;
            virtual void setAcceleration( const double& acceleration, bool relativeToModelSize = false );
            double getAcceleration( bool *relativeToModelSize = NULL ) const;
            virtual void setMaxVelocity( const double& maxVelocity, bool relativeToModelSize = false );
            double getMaxVelocity( bool *relativeToModelSize = NULL ) const;

            virtual void setWheelMovement( const double& wheelMovement, bool relativeToModelSize = false );
            double getWheelMovement( bool *relativeToModelSize = NULL ) const;

            virtual void home( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
            virtual void home( double );

            virtual void init( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

        protected:

            virtual bool handleMouseWheel( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

            virtual bool performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy );
            virtual bool performMouseDeltaMovement( const float dx, const float dy );
            virtual void applyAnimationStep( const double currentProgress, const double prevProgress );
            virtual bool startAnimationByMousePointerIntersection( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

            void moveForward( const double distance );
            void moveForward( const osg::Quat& rotation, const double distance );
            void moveRight( const double distance );
            void moveUp( const double distance );

            osg::Vec3d _eye;
            osg::Quat  _rotation;
            double _velocity;

            double _acceleration;
            static int _accelerationFlagIndex;
            double _maxVelocity;
            static int _maxVelocityFlagIndex;
            double _wheelMovement;
            static int _wheelMovementFlagIndex;

            class FirstPersonAnimationData : public AnimationData {
            public:
                osg::Quat _startRot;
                osg::Quat _targetRot;
                void start( const osg::Quat& startRotation, const osg::Quat& targetRotation, const double startTime );
            };
            virtual void allocAnimationData() { _animationData = new FirstPersonAnimationData(); }

    };


//
//  inline methods
//

/// Returns velocity.
    double TrackerManipulator::getVelocity() const  { return _velocity; }

};
#endif