#include "stdafx.h"
#ifndef _GRID_H
#define _GRID_H
#include <osgViewer/Viewer>
#include <osgText/Text>
#include <osg/Geometry>


class Grid : public osg::ShapeDrawable 
{
public:
    Grid(osg::Vec3 position = osg::Vec3(0.0, 0.0, 0.0), osg::Vec2 size = osg::Vec2(10.0 ,10.0), osg::Vec2 step = osg::Vec2(0.1, 0.1)) : osg::ShapeDrawable() 
    { 
        SetPosition(position.x(), position.y(), position.z());
        SetSize(size.x(), size.y());
        SetStep(step.x(), step.y());
    };
    Grid(osg::Shape* shape, osg::TessellationHints* hints=0) : osg::ShapeDrawable(shape,hints) { Init(); };
    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    Grid(const osg::ShapeDrawable& pg,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) : osg::ShapeDrawable(pg,copyop) { Init(); };


    virtual void drawImplementation(osg::RenderInfo & renderInfo) const;
    virtual osg::BoundingBox computeBound() const;
    void SetPosition(double posX, double posY, double posZ) { m_posX = posX; m_posY = posY; m_posZ = posZ;};
    void SetSize(double width, double height) { m_vWidth = width; m_vHeight = height; };
    void SetStep(double stepX, double stepY) { m_stepX = stepX; m_stepY = stepY; };
    
    void Init()
    {
        SetPosition(0.0f ,0.0f, 0.0f);
        SetSize(10.0f, 10.0f);
        SetStep(1.0f, 1.0f);
    }
protected:
    double m_posX;
    double m_posY;
    double m_posZ;
    double m_vWidth;
    double m_vHeight;
    double m_stepX;
    double m_stepY;
};
#endif