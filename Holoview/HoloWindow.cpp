#include "stdafx.h"
#include "HoloWindow.h"




HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
    m_picker = new PickHandler();

    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/Collada/duck.dae");
    osg::Node* selectionBox = m_picker->getOrCreateSelectionBox();
    root->addChild(m_picker->GetSelection());
    root->addChild(selectionBox);
    root->addChild(model);
    auto container = root->getOrCreateUserDataContainer(); //todo: use container to pass user data

    MultiViewerWidget* central = new MultiViewerWidget(this);

    osgViewer::Viewer viewer;

    central->SetStereoSettings();
    central->setMouseTracking(true);

    QDesktopWidget* desktop = QApplication::desktop();
    int numOfScreens = desktop->numScreens();

    central->CreateGraphicsWindow();
    central->setSceneData (root);
    central->addEventHandler(m_picker.get());
    central->addEventHandler(m_picker.get());
    central->setCameraManipulator( new osgGA::TrackballManipulator );


    /*for (int i = 0; i < numOfScreens; i++)
    {
        central->CreateGraphicsWindow();
        osgViewer::View* view = central->getView(i);
        view->setSceneData( root );
        view->addEventHandler( new osgViewer::StatsHandler );
        view->addEventHandler(m_picker.get());
        view->setCameraManipulator( new osgGA::TrackballManipulator );
        m_views.push_back(view);
    }*/
    setCentralWidget(central);
}

HoloWindow::~HoloWindow()
{

}


bool HoloWindow::AddOSGWidget()
{
    QString osgName = QString::fromLatin1("Scene View");
    OSGFrame *osgFrame = new OSGFrame(osgName, this);
    osgFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);

    MyDock *osgDock = new MyDock(osgName, this, Qt::WindowFlags(0), osgFrame);
    osgDock->setCustomSizeHint(m_customSizeHints.value("Scene View"));
    osgDock->setFloating(false);
    addDockWidget(Qt::RightDockWidgetArea, osgDock);
    dockWidgetMenu->addMenu(osgDock->menu);

	return true;
}


