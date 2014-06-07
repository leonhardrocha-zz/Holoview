#pragma once
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgFX/Technique>
#include <osgFX/Outline>

class PickHandler : public osgGA::GUIEventHandler
{
public:
    PickHandler(osg::Group* scene = NULL) : osgGA::GUIEventHandler()
    {
        m_selection = new osgFX::Outline;
        m_selection->setWidth( 8 );
        m_selection->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );
    }
    osgFX::Outline* GetSelection() { return m_selection; };
    osg::Node* getOrCreateSelectionBox();
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
protected:
    osg::ref_ptr<osg::MatrixTransform> m_selectionBox;
    osg::ref_ptr<osgFX::Outline> m_selection;

};