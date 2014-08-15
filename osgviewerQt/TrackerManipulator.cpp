#include "TrackerManipulator.h"
#include "IPose.h"
/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2010 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
 *
 * TrackerManipulator code Copyright (C) 2010 PCJohn (Jan Peciva)
 * while some pieces of code were taken from OSG.
 * Thanks to company Cadwork (www.cadwork.ch) and
 * Brno University of Technology (www.fit.vutbr.cz) for open-sourcing this work.
*/


#include <cassert>


using namespace osg;
using namespace osgGA;



int TrackerManipulator::_accelerationFlagIndex = allocateRelativeFlag();
int TrackerManipulator::_maxVelocityFlagIndex = allocateRelativeFlag();
int TrackerManipulator::_wheelMovementFlagIndex = allocateRelativeFlag();


/// Constructor.
TrackerManipulator::TrackerManipulator( int flags )
   : inherited( flags ),
     _velocity( 0. )
{
   setAcceleration( 1.0, true );
   setMaxVelocity( 0.25, true );
   setWheelMovement( 0.05, true );
   if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
      setAnimationTime( 0.2 );
}


/// Constructor.
TrackerManipulator::TrackerManipulator( const TrackerManipulator& fpm, const CopyOp& copyOp )
   : osg::Object(fpm, copyOp),
     inherited( fpm, copyOp ),
     _eye( fpm._eye ),
     _rotation( fpm._rotation ),
     _velocity( fpm._velocity ),
     _acceleration( fpm._acceleration ),
     _maxVelocity( fpm._maxVelocity ),
     _wheelMovement( fpm._wheelMovement )
{
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void TrackerManipulator::setByMatrix( const Matrixd& matrix )
{
   // set variables
   _eye = matrix.getTrans();
   _rotation = matrix.getRotate();

   // fix current rotation
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, true );
}


/** Set the position of the manipulator using a 4x4 matrix.*/
void TrackerManipulator::setByInverseMatrix( const Matrixd& matrix )
{
   setByMatrix( Matrixd::inverse( matrix ) );
}


/** Get the position of the manipulator as 4x4 matrix.*/
Matrixd TrackerManipulator::getMatrix() const
{
   return Matrixd::rotate( _rotation ) * Matrixd::translate( _eye );
}


/** Get the position of the manipulator as a inverse matrix of the manipulator,
    typically used as a model view matrix.*/
Matrixd TrackerManipulator::getInverseMatrix() const
{
   return Matrixd::translate( -_eye ) * Matrixd::rotate( _rotation.inverse() );
}


// doc in parent
void TrackerManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
   // set variables
   _eye = eye;
   _rotation = rotation;

   // fix current rotation
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, true );
}


// doc in parent
void TrackerManipulator::getTransformation( osg::Vec3d& eye, osg::Quat& rotation ) const
{
   eye = _eye;
   rotation = _rotation;
}


// doc in parent
void TrackerManipulator::setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up )
{
   // set variables
   osg::Matrixd m( osg::Matrixd::lookAt( eye, center, up ) );
   _eye = eye;
   _rotation = m.getRotate().inverse();

   // fix current rotation
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, true );
}


// doc in parent
void TrackerManipulator::getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const
{
   center = _eye + _rotation * osg::Vec3d( 0.,0.,-1. );
   eye = _eye;
   up = _rotation * osg::Vec3d( 0.,1.,0. );
}


/** Sets velocity.
 *
 *  There are no checks for maximum velocity applied.
 */
void TrackerManipulator::setVelocity( const double& velocity )
{
   _velocity = velocity;
}

void TrackerManipulator::setTrackingResults( IArgs* results, osg::Vec3 center)
{
    const osg::Vec3 kinectBasePosition(0.0, 0.615, 0.1);
    const double kinectTiltAngle = osg::inDegrees(20.0);
    const osg::Vec3 kinectEyeOffset(0.0, 0.06, 0.0);
    const osg::Vec3 kinectEyePosition = kinectBasePosition + osg::Matrix::rotate(-kinectTiltAngle, osg::Vec3(1.0, 0.0, 0.0)) * kinectEyeOffset;

    IPose *position = static_cast<IPose*>(results->Get("position"));

    osg::Vec3 avatarPosition(position->Get(0), position->Get(1), position->Get(2));
    osg::Vec3 kinectFrustumOffset( 0.0, avatarPosition.y() * sin(kinectTiltAngle), avatarPosition.z() * cos(kinectTiltAngle) );
    osg::Vec3 kinectFrustumTargetPosition = kinectEyePosition + kinectFrustumOffset;
    osg::Vec3 eye(avatarPosition.x(), avatarPosition.y() + kinectFrustumTargetPosition.y(), avatarPosition.z());
    _rotation = osg::Matrix::lookAt(eye, center, osg::Vec3( 0.,1.,0. )).getRotate();
    _eye = eye;
}
/** Sets acceleration.
 *
 *  If acceleration effect is unwanted, it can be set to DBL_MAX.
 *  Then, there will be no acceleration and object will reach its
 *  maximum velocity immediately.
 */
void TrackerManipulator::setAcceleration( const double& acceleration, bool relativeToModelSize )
{
   _acceleration = acceleration;
   setRelativeFlag( _accelerationFlagIndex, relativeToModelSize );
}


/// Returns acceleration speed.
double TrackerManipulator::getAcceleration( bool *relativeToModelSize ) const
{
   if( relativeToModelSize )
      *relativeToModelSize = getRelativeFlag( _accelerationFlagIndex );

   return _acceleration;
}


/** Sets maximum velocity.
 *
 *  If acceleration is set to DBL_MAX, there is no speeding up.
 *  Instead, maximum velocity is used for velocity at once without acceleration.
 */
void TrackerManipulator::setMaxVelocity( const double& maxVelocity, bool relativeToModelSize )
{
   _maxVelocity = maxVelocity;
   setRelativeFlag( _maxVelocityFlagIndex, relativeToModelSize );
}


/// Returns maximum velocity.
double TrackerManipulator::getMaxVelocity( bool *relativeToModelSize ) const
{
   if( relativeToModelSize )
      *relativeToModelSize = getRelativeFlag( _maxVelocityFlagIndex );

   return _maxVelocity;
}


/// Sets movement size on single wheel step.
void TrackerManipulator::setWheelMovement( const double& wheelMovement, bool relativeToModelSize )
{
   _wheelMovement = wheelMovement;
   setRelativeFlag( _wheelMovementFlagIndex, relativeToModelSize );
}


/// Returns movement size on single wheel step.
double TrackerManipulator::getWheelMovement( bool *relativeToModelSize ) const
{
   if( relativeToModelSize )
      *relativeToModelSize = getRelativeFlag( _wheelMovementFlagIndex );

   return _wheelMovement;
}


void TrackerManipulator::home( double currentTime )
{
   inherited::home( currentTime );
   _velocity = 0.;
}


void TrackerManipulator::home( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
   inherited::home( ea, us );
   _velocity = 0.;
}


void TrackerManipulator::init( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
   inherited::init( ea, us );

   // stop movement
   _velocity = 0.;
}


// doc in parent
bool TrackerManipulator::handleMouseWheel( const GUIEventAdapter& ea, GUIActionAdapter& us )
{
    osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();

    // handle centering
    if( _flags & SET_CENTER_ON_WHEEL_FORWARD_MOVEMENT )
    {

        if( ((sm == GUIEventAdapter::SCROLL_DOWN) && (_wheelMovement > 0.)) ||
            ((sm == GUIEventAdapter::SCROLL_UP)   && (_wheelMovement < 0.)) )
        {

            // stop thrown animation
            _thrown = false;

            if( getAnimationTime() <= 0. )

                // center by mouse intersection (no animation)
                setCenterByMousePointerIntersection( ea, us );

            else {

                // start new animation only if there is no animation in progress
                if( !isAnimating() )
                    startAnimationByMousePointerIntersection( ea, us );

            }
        }
    }

    switch( sm )
    {

        // mouse scroll up event
        case GUIEventAdapter::SCROLL_UP:
        {
            // move forward
            moveForward( isAnimating() ? dynamic_cast< FirstPersonAnimationData* >( _animationData.get() )->_targetRot : _rotation,
                         -_wheelMovement * (getRelativeFlag( _wheelMovementFlagIndex ) ? _modelSize : 1. ));
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // mouse scroll down event
        case GUIEventAdapter::SCROLL_DOWN:
        {
            // move backward
            moveForward( _wheelMovement * (getRelativeFlag( _wheelMovementFlagIndex ) ? _modelSize : 1. ));
            _thrown = false;
            us.requestRedraw();
            us.requestContinuousUpdate( isAnimating() || _thrown );
            return true;
        }

        // unhandled mouse scrolling motion
        default:
            return false;
    }
}


// doc in parent
bool TrackerManipulator::performMovementLeftMouseButton( const double /*eventTimeDelta*/, const double dx, const double dy )
{
   // world up vector
   CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
   Vec3d localUp = getUpVector( coordinateFrame );

   rotateYawPitch( _rotation, dx, dy, localUp );

   return true;
}


bool TrackerManipulator::performMouseDeltaMovement( const float dx, const float dy )
{
   // rotate camera
   if( getVerticalAxisFixed() ) {

      // world up vector
      CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
      Vec3d localUp = getUpVector( coordinateFrame );

      rotateYawPitch( _rotation, dx, dy, localUp );

   } else

      rotateYawPitch( _rotation, dx, dy );

   return true;
}


/// Move camera forward by distance parameter.
void TrackerManipulator::moveForward( const double distance )
{
   moveForward( _rotation, distance );
}


/// Move camera forward by distance parameter.
void TrackerManipulator::moveForward( const Quat& rotation, const double distance )
{
   _eye += rotation * Vec3d( 0., 0., -distance );
}


/// Move camera right by distance parameter.
void TrackerManipulator::moveRight( const double distance )
{
   _eye += _rotation * Vec3d( distance, 0., 0. );
}


/// Move camera up by distance parameter.
void TrackerManipulator::moveUp( const double distance )
{
   _eye += _rotation * Vec3d( 0., distance, 0. );
}


void TrackerManipulator::applyAnimationStep( const double currentProgress, const double /*prevProgress*/ )
{
   FirstPersonAnimationData *ad = dynamic_cast< FirstPersonAnimationData* >( _animationData.get() );
   assert( ad );

   // compute new rotation
   _rotation.slerp( currentProgress, ad->_startRot, ad->_targetRot );

   // fix vertical axis
   if( getVerticalAxisFixed() )
      fixVerticalAxis( _eye, _rotation, false );
}


// doc in parent
bool TrackerManipulator::startAnimationByMousePointerIntersection(
      const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
   // get current transformation
   osg::Vec3d prevEye;
   osg::Quat prevRot;
   getTransformation( prevEye, prevRot );

   // center by mouse intersection
   if( !setCenterByMousePointerIntersection( ea, us ) )
      return false;

   FirstPersonAnimationData *ad = dynamic_cast< FirstPersonAnimationData*>( _animationData.get() );
   assert( ad );

   // setup animation data and restore original transformation
   ad->start( prevRot, _rotation, ea.getTime() );
   setTransformation( _eye, prevRot );

   return true;
}


void TrackerManipulator::FirstPersonAnimationData::start( const Quat& startRotation, const Quat& targetRotation,
                                                              const double startTime )
{
   AnimationData::start( startTime );

   _startRot = startRotation;
   _targetRot = targetRotation;
}
