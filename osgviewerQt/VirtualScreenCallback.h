#include "stdafx.h"
#ifndef _VIRTUALSCREENCALLBACK_H
#define _VIRTUALSCREENCALLBACK_H

class VirtualScreenCallback : public osg::NodeCallback
{
public:
VirtualScreenCallback( osg::PositionAttitudeTransform* billboard, const osg::Vec3& screenOffset, osg::CameraView* cameraView)
: m_screenOffset(screenOffset), m_billboard(billboard), m_modelOffset(billboard->getPosition()), m_cameraView(cameraView) {}

virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
    if ( !cv ) return;
    //m_cameraView->setPosition( nv->getEyePoint() );
    m_billboard->setPosition( m_modelOffset + m_screenOffset );
    traverse( node, nv );
}
protected:
    osg::observer_ptr<osg::PositionAttitudeTransform> m_billboard;
    osg::observer_ptr<osg::CameraView> m_cameraView;
    const osg::Vec3 m_screenOffset;
    const osg::Vec3 m_modelOffset;
};
#endif