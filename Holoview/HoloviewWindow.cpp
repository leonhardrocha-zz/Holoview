#include "stdafx.h"
#include "HoloviewWindow.h"

void HoloviewWindow::ResetWindow()
{
	SetupWindow(width(), height());
}

void HoloviewWindow::SetupWindow(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
}


void HoloviewWindow::initialize()
{
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/Collada/duck.dae");
    osg::ref_ptr<osg::Group> modelGroup = new osg::Group;
    modelGroup->addChild(model);

    osg::Node* selectionBox = m_osgPicker->getOrCreateSelectionBox();

    m_osgScene->addChild(m_osgPicker->GetSelection());
    m_osgScene->addChild(selectionBox);
    m_osgScene->addChild(modelGroup);

    m_osgView->setSceneData( m_osgScene );
    m_osgView->addEventHandler( new osgViewer::StatsHandler );
    m_osgView->addEventHandler(m_osgPicker.get());
    m_osgView->setCameraManipulator( new osgGA::TrackballManipulator );

    if (m_tracker)
    {
        m_tracker->SetTrackerCallback(HoloviewWindow::TrackerUpdateStatic, this);
    }
}


void HoloviewWindow::render()
{
	if (!m_pResults)
	{
		return;
	}
}

void HoloviewWindow::SetupScene()
{
}

void HoloviewWindow::RenderScene()
{
}

void HoloviewWindow::TrackerUpdateStatic(void* lpParam, void* args)
{
	HoloviewWindow* pThis = reinterpret_cast<HoloviewWindow*>(lpParam);
	TrackingResults* results = static_cast<TrackingResults*>(args);
	pThis->SetTrackingResults(results->trackerId, results);
	pThis->render();
}

void HoloviewWindow::SetTrackingResults(int trackerId, TrackingResults* results)
{	
	m_pResults = results;
}

void HoloviewWindow::resizeWindow()
{
	ResetWindow();
	render();
}