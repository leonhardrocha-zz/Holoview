#include "PickerHandler.h"

osg::Node* PickHandler::getOrCreateSelectionBox()
{
	if ( !_selectionBox )
	{
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(	new osg::ShapeDrawable(new osg::Box(osg::Vec3(),1.0f)) );
		_selectionBox = new osg::MatrixTransform;
		_selectionBox->setNodeMask( 0x1 );
		_selectionBox->addChild( geode.get() );
		osg::StateSet* ss = _selectionBox->getOrCreateStateSet();
		ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		ss->setAttributeAndModes(new osg::PolygonMode(
			osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE));
	}
	return _selectionBox.get();
}

bool PickHandler::handle( const osgGA::GUIEventAdapter& ea,
	osgGA::GUIActionAdapter& aa )
{
	if ( ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE ||	
		 ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ||
		 !(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) )
	{
		return false;
	}
	
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if ( viewer )
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =	new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
		osgUtil::IntersectionVisitor iv( intersector.get() );
		iv.setTraversalMask( ~0x1 );
		viewer->getCamera()->accept( iv );
		if ( intersector->containsIntersections() )
		{
			osgUtil::LineSegmentIntersector::Intersection result =*(intersector->getIntersections().begin());
			osg::BoundingBox bb = result.drawable->getBound();
			osg::Vec3 worldCenter = bb.center() * osg::computeLocalToWorld(result.nodePath);
			_selectionBox->setMatrix(osg::Matrix::scale(bb.xMax()-bb.xMin(),
									 bb.yMax()-bb.yMin(),
									 bb.zMax()-bb.zMin()) *
									 osg::Matrix::translate(worldCenter) );

		}
	}
	return false;
}