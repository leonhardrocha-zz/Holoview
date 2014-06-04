#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/Viewer>

class PickHandler : public osgGA::GUIEventHandler
{
	public:
	osg::Node* getOrCreateSelectionBox();
	virtual bool handle( const osgGA::GUIEventAdapter& ea,
	osgGA::GUIActionAdapter& aa );
	protected:
	osg::ref_ptr<osg::MatrixTransform> _selectionBox;
};