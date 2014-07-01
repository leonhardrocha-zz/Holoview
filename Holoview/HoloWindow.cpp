#include "stdafx.h"
#include "HoloWindow.h"

HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
   

    osg::ref_ptr<osg::Group> root = new osg::Group;
    /*osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/Collada/duck.dae.-90,-90,0.rot");*/
    /*osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/3ds/airplane/Airplane AN-2 N200314.3DS.-90,-10,0.rot");*/
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/osg/cessna.osgt.-90,0,0.rot");

    auto container = root->getOrCreateUserDataContainer(); //todo: use container to pass user data

    MultiViewerWidget* fullScreen = new MultiViewerWidget(this);

    fullScreen->addEventHandler( new SelectModelHandler );

    fullScreen->SetStereoSettings();
    fullScreen->setMouseTracking(true);

    QDesktopWidget* desktop = QApplication::desktop();
    int numOfScreens = desktop->numScreens();

    fullScreen->CreateGraphicsWindow();
    fullScreen->setCameraManipulator( new osgGA::TrackerManipulator );
    osg::BoundingSphere bSphere = model->computeBound();
    osg::Matrix m;
    double sceneRadius = 0.25; // 3 feet = 3 * 12 * inch in meters (SI)
    double modelScale = sceneRadius * 1.0/bSphere.radius();

    osg::Vec3 translationToModel = -bSphere.center();
    osg::Vec3 translationToWorld(0.0, 0.5, 1.0);
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform(m.translate(translationToModel) * m.scale(modelScale, modelScale, modelScale) * m.translate(translationToWorld) );
    transform->addChild(model);
    root->addChild(transform);
    fullScreen->setSceneData(root);
    m_view = fullScreen->getViewerBase();
    AddSkyBox();
}

HoloWindow::~HoloWindow()
{

}

void HoloWindow::AddSkyBox()
{
    MultiViewerWidget* fullScreen = static_cast<MultiViewerWidget*>(m_view);
    osg::ref_ptr<osg::Node> scene = fullScreen->getSceneData();
    
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), scene->getBound().radius())) );
    geode->setCullingActive( false );
    
    osg::ref_ptr<SkyBox> skybox = new SkyBox;
    skybox->getOrCreateStateSet()->setTextureAttributeAndModes( 0, new osg::TexGen );
    /*osg::ref_ptr<osg::Node> land = osgDB::readNodeFile("../Dependencies/Models/OSG/lz.osg.-90,-90,0.rot");*/

    std::string path = "../Dependencies/Images/Cubemap_snow/";
    std::string ext = ".jpg";
    skybox->setEnvironmentMap( 0,
        osgDB::readImageFile(path + "posx" + ext), osgDB::readImageFile(path + "negx" + ext),
        osgDB::readImageFile(path + "posy" + ext), osgDB::readImageFile(path + "negy" + ext),
        osgDB::readImageFile(path + "posz" + ext), osgDB::readImageFile(path + "negz" + ext) );
    skybox->addChild( geode.get() );
    
    osg::ref_ptr<osg::Group> root = new osg::Group;
    /*root->addChild( land.get() );*/
    root->addChild( scene.get() );
    root->addChild( skybox.get() );

    fullScreen->setSceneData( root.get() );
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


