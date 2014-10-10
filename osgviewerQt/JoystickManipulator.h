#ifndef _JOYSTICKMANIPULATOR_H
#define _JOYSTICKMANIPULATOR_H

#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include <osgGA/StandardManipulator>

namespace osgGA {
    class JoystickManipulator : public StandardManipulator
    {
    public:
        JoystickManipulator();
    
        virtual osg::Matrixd getMatrix() const;
        virtual osg::Matrixd getInverseMatrix() const;
        virtual void setByMatrix( const osg::Matrixd& matrix );
        virtual void setByInverseMatrix( const osg::Matrixd& matrix );
    
        virtual void setTransformation( const osg::Vec3d&, const osg::Quat& ) { }
        virtual void setTransformation( const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& viewup) { }
        virtual void getTransformation( osg::Vec3d& eye, osg::Quat& rotation) const { osg::Vec3 t,s; osg::Quat so; getMatrix().decompose(t, rotation, s, so); eye = -t;}
        virtual void getTransformation( osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up ) const { getMatrix().getLookAt(eye, center, up); }
    
        virtual void init( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
        virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    
        virtual void home( double );
        virtual void home( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
        { home(ea.getTime()); }
    
    protected:
        virtual ~JoystickManipulator();
    
        virtual bool performMovementLeftMouseButton(
            const double eventTimeDelta, const double dx, const double dy );
        virtual bool performMovementRightMouseButton(
            const double eventTimeDelta, const double dx, const double dy );
    
        osg::Vec3 _center;
        double _distance;
    };
}
#endif
