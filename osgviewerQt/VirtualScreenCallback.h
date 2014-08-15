#include "osgLib.h"
#ifndef _VIRTUALSCREENCALLBACK_H
#define _VIRTUALSCREENCALLBACK_H

#include "DualScreenViewer.h"

#define defaultScreenOffset 1.0

class VirtualScreenCallback : public osg::NodeCallback
{
public:
    VirtualScreenCallback(osg::CullSettings::ClampProjectionMatrixCallback* callback) : m_callback(callback)
    {

    }

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if ( !cv ) return;
        cv->setClampProjectionMatrixCallback(m_callback);
        traverse( node, nv );
    }
    protected:
        osg::CullSettings::ClampProjectionMatrixCallback* m_callback;
};
#endif