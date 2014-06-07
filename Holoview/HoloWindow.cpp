#include "stdafx.h"
#include "HoloWindow.h"
#include "MainWindow.h"

HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    ds->setStereo(true);
    ds->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    ds->setDisplayType(osg::DisplaySettings::MONITOR);
    ds->setSplitStereoHorizontalEyeMapping(osg::DisplaySettings::LEFT_EYE_LEFT_VIEWPORT);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = "HoloWindow";
    traits->windowDecoration = false;
    traits->x = 0;
    traits->y = 0;
    traits->width = 800;
    traits->height = 600;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    m_osgPicker = new PickHandler();
    m_osgView = new osgViewer::View;
    m_osgScene = new osg::Group;
    
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


    ViewerWidget* central = new ViewerWidget(parent);
    central->SetView(m_osgView);
    central->CreateGraphicsWindow(ds, traits);
    setCentralWidget(central);
    central->setMouseTracking(true);
    central->Init();
}

HoloWindow::~HoloWindow()
{

}



