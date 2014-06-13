#include "stdafx.h"
#include "HoloWindow.h"




HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
    //float tvSizeWidth = 0.305 * 5; // 5 feet
    //float tvSizeHeight = 0.305 * 3; // 3 feet
    //float tvSizeDepth = 1.5 * 0.0254; // 1.5 inches
    //float tvPadHeight = 0.07; // 7 cm
    //float tvScreenWidth = 56 * 0.0254; // 56 inches
    //float tvScreenHeight = 32 * 0.0254; // 32 inches
    //float tvScreenDepth = 0 ; // 0

    //// world widget

    //ViewerWidget* central = new ViewerWidget(this);

    //osg::ref_ptr<osg::Group> worldRoot= new osg::Group;

    //float rotationAngle =  osg::DegreesToRadians(30.0f);
    //float offsetX = tvSizeWidth * std::cos(rotationAngle) /2;
    //float offsetZ = tvSizeWidth * std::sin(rotationAngle);

    //osg::ref_ptr<osg::Node> tv = osgDB::readNodeFile("../Dependencies/Models/3ds/TV/tv.3ds");


    //// left tv


    //osg::ref_ptr<osg::MatrixTransform> leftTransform= new osg::MatrixTransform();
    //osg::Matrix leftMatrix = leftTransform->getMatrix();
    //leftTransform->setMatrix( leftMatrix.scale(0.01f, 0.01f, 0.01f) * leftMatrix.rotate(rotationAngle, osg::Vec3(0,0,1)) * leftMatrix.translate(-offsetX, 0, offsetZ) );
    //leftTransform->addChild(tv);
    //worldRoot->addChild(leftTransform);


    //// right tv

    //osg::ref_ptr<osg::MatrixTransform> rightTransform= new osg::MatrixTransform();
    //osg::Matrix rightMatrix = rightTransform->getMatrix();
    //rightTransform->setMatrix( rightMatrix.scale(0.01f, 0.01f, 0.01f) * rightMatrix.rotate(-rotationAngle, osg::Vec3(0,0,1)) * rightMatrix.translate(offsetX,  0, offsetZ) );
    //rightTransform->addChild(tv);
    //worldRoot->addChild(rightTransform);

    //// rest


    //central->CreateGraphicsWindow();
    //osgViewer::View* view = central->getView(0);
    //view->addEventHandler( new osgViewer::StatsHandler );
    //view->setCameraManipulator( new osgGA::TrackballManipulator );
    //view->setSceneData( worldRoot );
    //setCentralWidget(central);

    // full screen

    m_picker = new PickHandler();
    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/Collada/duck.dae");
    root->addChild(model);

    auto container = root->getOrCreateUserDataContainer(); //todo: use container to pass user data

    MultiViewerWidget* fullScreen = new MultiViewerWidget(this);

    fullScreen->SetStereoSettings();
    fullScreen->setMouseTracking(true);

    QDesktopWidget* desktop = QApplication::desktop();
    int numOfScreens = desktop->numScreens();

    fullScreen->CreateGraphicsWindow();
    fullScreen->setCameraManipulator( new osgGA::TrackballManipulator );
    fullScreen->setSceneData(root);
    //osgViewer::ViewerBase::Cameras cameras;
    //fullScreen->getCameras(cameras);

    /*cameras[0]->setViewMatrixAsLookAt();*/


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


