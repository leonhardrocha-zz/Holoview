#include "stdafx.h"
#ifndef _GRID_H
#define _GRID_H
class Grid : public osg::ShapeDrawable 
{
public:
    Grid() : osg::ShapeDrawable() { Init(); };
    Grid(osg::Shape* shape, osg::TessellationHints* hints=0) : osg::ShapeDrawable(shape,hints) { Init(); };
    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    Grid(const osg::ShapeDrawable& pg,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) : osg::ShapeDrawable(pg,copyop) { Init(); };

    virtual void drawImplementation(osg::RenderInfo & renderInfo) const;
    virtual osg::BoundingBox computeBound() const;
    void SetPosition(float posX, float posY) { m_posX = posX; m_posY = posY; };
    void SetSize(float width, float height) { m_vWidth = width; m_vHeight = height; };
    void SetZoom(float zoom) { m_zoom = zoom; };
    void Init()
    {
        SetPosition(0.0f ,0.0f);
        SetSize(300.0f, 300.0f);
        SetZoom(1.0f);
    }
protected:
    float m_posX;
    float m_posY;
    float m_vWidth;
    float m_vHeight;
    float m_zoom;
};
#endif