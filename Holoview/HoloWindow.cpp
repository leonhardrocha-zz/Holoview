#include "stdafx.h"
#include "HoloWindow.h"

HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
   

    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/Collada/duck.dae.-90,-90,0.rot");
    /*osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/3ds/airplane/Airplane AN-2 N200314.3DS.-90,-10,0.rot");*/
    /*osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../Dependencies/Models/osg/cessna.osgt.-90,0,0.rot");*/
    osg::ref_ptr<osg::Node> kinect = osgDB::readNodeFile("../Dependencies/Models/3ds/kinect/kinect_edited.3ds");
    auto container = root->getOrCreateUserDataContainer(); //todo: use container to pass user data

    MultiViewerWidget* fullScreen = new MultiViewerWidget(this);

    fullScreen->addEventHandler( new SelectModelHandler );

    fullScreen->SetStereoSettings();
    fullScreen->setMouseTracking(true);

    QDesktopWidget* desktop = QApplication::desktop();
    int numOfScreens = desktop->numScreens();

    fullScreen->CreateGraphicsWindow();
    fullScreen->setCameraManipulator( new osgGA::TrackerManipulator );
    osg::Vec3 modelPosition(0.0, 0.5, 1.0);
    osg::Vec3 kinectPosition(0.0, -0.1, 0.0);
    root->addChild(GetModelTransformHelper(model, modelPosition, 0.25));
    root->addChild(GetModelTransformHelper(kinect, kinectPosition));
    fullScreen->setSceneData(root);
    m_view = fullScreen->getViewerBase();
    AddSkyBox();
}

HoloWindow::~HoloWindow()
{

}

osg::ref_ptr<osg::MatrixTransform> HoloWindow::GetModelTransformHelper(const osg::ref_ptr<osg::Node> model, const osg::Vec3& worldPosition, const double modelRadius)
{
    osg::BoundingSphere bSphere = model->computeBound();
    osg::Vec3 translationToModel = -bSphere.center();
    osg::Matrix m = osg::Matrix::translate(translationToModel);
    if (modelRadius != 0.0) {
        double modelScale = modelRadius * 1.0/bSphere.radius();
        m = m * osg::Matrix::scale(modelScale, modelScale, modelScale);
    }
    m = m * osg::Matrix::translate(worldPosition);
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform(m);
    transform->addChild(model);
    return transform;
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
    std::string name = "axis";
    std::string path = "../Dependencies/Images/Cubemap_" + name + "/";
    std::string ext = ".png";
    std::string sign[] = { "pos", "neg" };
    std::string axis[] = { "x", "y", "z" };
    std::vector<osg::Image*> images;
    for (int i=0; i < 6; i++)
    {
        std::string filename = path + sign[i%2] + axis[i/2] + ext;
        osg::Image* image = osgDB::readImageFile(filename);
        //image->flipHorizontal();
        images.push_back(image);
    }

    skybox->setEnvironmentMap( 0, images[0], images[1], images[2], images[3], images[4], images[5]);

    skybox->addChild( geode.get() );
    
    osg::ref_ptr<osg::Group> root = new osg::Group;
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


