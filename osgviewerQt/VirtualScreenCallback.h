#include "stdafx.h"
#ifndef _VIRTUALSCREENCALLBACK_H
#define _VIRTUALSCREENCALLBACK_H

#include "DualScreenViewer.h"

class VirtualScreenCallback : public osg::NodeCallback
{
public:
VirtualScreenCallback( osg::PositionAttitudeTransform* billboard, const osg::Vec3& modelPosition, const osg::Vec3& screenOffset, DualScreenViewer* viewer)
: m_screenOffset(screenOffset), m_billboard(billboard), m_modelPosition(modelPosition), m_viewer(viewer) 
{
    osg::Vec3 eyePosition = m_viewer->GetVirtualEye() + m_viewer->GetVirtualCenter();
    osg::Matrix attitude = osg::Matrix::lookAt( eyePosition, m_viewer->GetVirtualCenter(), osg::Vec3(0,1,0));
    m_modelAttitude = attitude.getRotate();
}

virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
    if ( !cv ) return;
    m_billboard->setPosition( m_modelPosition );
    osg::Vec3 holoPosition = m_modelPosition - m_viewer->GetVirtualOrigin();
    m_viewer->getDisplaySettings()->setScreenDistance( holoPosition.length() );
    osg::Vec3 eyePosition = m_viewer->GetVirtualEye() + m_viewer->GetVirtualCenter();
    osg::Quat r = osg::Matrix::lookAt( eyePosition, m_viewer->GetVirtualOrigin(), osg::Vec3(0,1,0)).getRotate();
    m_billboard->setAttitude( r * m_modelAttitude );
    traverse( node, nv );
}
protected:
    osg::observer_ptr<osg::PositionAttitudeTransform> m_billboard;
    osg::observer_ptr<DualScreenViewer> m_viewer;
    const osg::Vec3 m_screenOffset;
    const osg::Vec3 m_modelPosition;
    osg::Quat m_modelAttitude;
};
#endif