#include "PickHandler.h"


osg::Node* PickHandler::getOrCreateSelectionBox()
{
	if ( !m_selectionBox )
	{
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(	new osg::ShapeDrawable(new osg::Box(osg::Vec3(),1.0f)) );
		m_selectionBox = new osg::MatrixTransform;
		m_selectionBox->setNodeMask( 0x1 );
		m_selectionBox->addChild( geode.get() );
		osg::StateSet* ss = m_selectionBox->getOrCreateStateSet();
		ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		ss->setAttributeAndModes(new osg::PolygonMode(
		osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE));
	}
	return m_selectionBox.get();
}

bool PickHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	if ( ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE ||	
		 ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ||
		 !(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) )
	{
		return false;
	}
	
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
	if ( viewer )
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =	new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
		osgUtil::IntersectionVisitor iv( intersector.get() );
		iv.setTraversalMask( ~0x1 );
        auto camera = viewer->getCamera();
		camera->accept( iv );
		if ( intersector->containsIntersections() )
		{
			osgUtil::LineSegmentIntersector::Intersection result =*(intersector->getIntersections().begin());
			osg::BoundingBox bb = result.drawable->getBound();
			osg::Vec3 worldCenter = bb.center() * osg::computeLocalToWorld(result.nodePath);
            m_selectionBox->setMatrix(osg::Matrix::scale(bb.xMax()-bb.xMin(),
									                     bb.yMax()-bb.yMin(),
									                     bb.zMax()-bb.zMin()) *
									                     osg::Matrix::translate(worldCenter) );
            auto clearMask = camera->getClearMask();
            osg::DisplaySettings::instance()->setMinimumNumStencilBits(1); 
            camera->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT); 
            camera->setClearStencil(0);
            int last = result.nodePath.size()-1;
            auto node = result.nodePath[last];
            if (node)
            {
                m_selection->addChild(node);
            }
        } 
	}
	return false;
}