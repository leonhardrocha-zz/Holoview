#include "stdafx.h"
#ifndef _SELECTMODELHANDLER_H
#define _SELECTMODELHANDLER_H

#include "CommonFunctions.h"

const osg::Vec4 normalColor(1.0f, 1.0f, 1.0f, 1.0f);
const osg::Vec4 selectedColor(1.0f, 0.0f, 0.0f, 0.5f);

class SelectModelHandler : public osgCookBook::PickHandler
{
public:
    SelectModelHandler() : _lastDrawable(0) {}
    
    virtual void doUserOperations( osgUtil::LineSegmentIntersector::Intersection& result )
    {
        if ( _lastDrawable.valid() )
        {
            setDrawableColor( _lastDrawable.get(), normalColor );
            _lastDrawable = NULL;
        }
        
        osg::Geometry* geom = dynamic_cast<osg::Geometry*>( result.drawable.get() );
        if ( geom )
        {
            setDrawableColor( geom, selectedColor );
            _lastDrawable = geom;
        }
    }
    
    void setDrawableColor( osg::Geometry* geom, const osg::Vec4& color )
    {
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>( geom->getColorArray() );
        if ( colors && colors->size()>0 )
        {
            colors->front() = color;
            colors->dirty();
        }
    }
    
protected:
    osg::observer_ptr<osg::Geometry> _lastDrawable;
};
#endif