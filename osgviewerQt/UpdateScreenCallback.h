#include "osgLib.h"
#ifndef _UPDATESCREENCALLBACK_H
#define _UPDATESCREENCALLBACK_H


class UpdateScreenCallback : public osg::NodeCallback
{
public:
    UpdateScreenCallback(osg::Camera* camera, osg::MatrixTransform* cameraTransform) : m_camera(camera), m_cameraTransform(cameraTransform)
    {
    }

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        osgUtil::UpdateVisitor* uv = dynamic_cast<osgUtil::UpdateVisitor*>(nv);
        if ( !uv ) return;
        if (node == m_cameraTransform)
        {
            m_cameraTransform->setMatrix(m_camera->getInverseViewMatrix());
        }

        traverse( node, nv );
    }
    protected:
        osg::ref_ptr<osg::Camera> m_camera;
        osg::MatrixTransform* m_cameraTransform;
};
#endif