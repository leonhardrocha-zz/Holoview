#include "qtLib.h"
#include "HoloWindow.h"

HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
    : MainWindow(customSizeHints, parent, flags)
{
    osg::ref_ptr<osg::Node> duck = osgDB::readNodeFile("../Dependencies/Models/Collada/duck.dae.-90,-90,0.rot");
    osg::ref_ptr<osg::Node> airplane = osgDB::readNodeFile("../Dependencies/Models/3ds/airplane/Airplane AN-2 N200314.3DS.-90,-100,0.rot");
    //osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("../Dependencies/Models/3ds/TargetCameraAnim.3ds.-90,0,0.rot");
    //osg::ref_ptr<osg::Node> kinect = osgDB::readNodeFile("../Dependencies/Models/3ds/kinect/kinect_edited.3ds.-20,0,0.rot");

    osg::Vec3 modelPosition1(0.50, 1.0, 0.75);
    osg::Vec3 modelPosition2(-0.50, 1.0, 0.75);
    //osg::Vec3 modelPosition3(0.25, 0.0,  0);
    //osg::Vec3 modelPosition4(-0.25, 0.0, 0);
    //osg::Vec3 modelPosition5(0.0, 0.0, 0.30); //kinect

    osg::ref_ptr<osg::PositionAttitudeTransform> transform1 = GetModelTransformHelper(duck, modelPosition1);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform2 = GetModelTransformHelper(airplane, modelPosition2);
    //osg::ref_ptr<osg::PositionAttitudeTransform> transform3 = GetModelTransformHelper(cessna, modelPosition3);
    //osg::ref_ptr<osg::PositionAttitudeTransform> transform4 = GetModelTransformHelper(duck, modelPosition4);
    //osg::ref_ptr<osg::PositionAttitudeTransform> transform5 = GetModelTransformHelper(kinect, modelPosition5);

    osg::ref_ptr<osg::Group> root = new osg::Group();
    osg::ref_ptr<osg::Group> scene = new osg::Group();
    
    scene->addChild(transform1);
    scene->addChild(transform2);
    //scene->addChild(transform3);
    //scene->addChild(transform4);
    //scene->addChild(transform5);

    m_viewer = new DualScreenViewer(true);

    // View 0 -- Just the loaded model.

    osgViewer::View* mainView = m_viewer->GetMainView();
    mainView->setSceneData( scene.get() );
    AddGrid( mainView );
    AddSkyBox( mainView , "snow");

    // View 1 -- Contains the loaded model, as well as a wireframe frustum derived from View 0's Camera.

    osgViewer::View* mapView = m_viewer->GetMapView();
    root->addChild( scene.get() );
    root->addChild( m_viewer->makeFrustumFromCamera( mainView ) );
    mapView->setSceneData( root.get() );
}

HoloWindow::~HoloWindow()
{

}

osg::ref_ptr<osg::PositionAttitudeTransform> HoloWindow::GetModelTransformHelper(const osg::ref_ptr<osg::Node> model, const osg::Vec3 modelPosition, const osg::Quat modelAttitude, const double modelRadius)
{
    osg::BoundingSphere bSphere = model->computeBound();
    osg::Vec3 translationToModel = -bSphere.center();
    osg::Matrix m = osg::Matrix::translate(translationToModel);
    double modelScale = modelRadius != 0.0 ? modelRadius * 1.0/bSphere.radius() : 1.0;
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
    transform->setScale(osg::Vec3(modelScale, modelScale, modelScale));
    transform->setPosition(osg::Vec3(modelPosition.x(), modelPosition.y(), modelPosition.z()));
    transform->setAttitude(modelAttitude);
    transform->addChild(model);
    return transform;
}

void HoloWindow::AddGrid(osgViewer::View* view)
{
    osg::Node* scene = view->getSceneData();
    
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable( new Grid(osg::Vec3(0.0, 0.0, 0.0), osg::Vec2(25.0, 25.0), osg::Vec2(0.25, 0.25) ) );
    geode->setCullingActive( false );

    osg::Group* root = new osg::Group;
    osg::MatrixTransform* transform = new osg::MatrixTransform(osg::Matrix::rotate(-osg::PI_2, osg::Vec3(1,0,0)));
    transform->addChild( geode );
    root->addChild( scene );
    root->addChild( transform );
    view->setSceneData( root );
}

void HoloWindow::AddSkyBox(osgViewer::View* view, std::string name)
{
    osg::Node* scene = view->getSceneData();

    osg::Geode* geode = new osg::Geode();
    geode->addDrawable( new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), scene->getBound().radius())) );
    geode->setCullingActive( false );

    SkyBox* skybox = new SkyBox();
    skybox->getOrCreateStateSet()->setTextureAttributeAndModes( 0, new osg::TexGen );

    skybox->setEnvironmentMap( 0, 
                                m_skybox.ImageList[name][pos_x],
                                m_skybox.ImageList[name][neg_x],
                                m_skybox.ImageList[name][pos_y],
                                m_skybox.ImageList[name][neg_y],
                                m_skybox.ImageList[name][pos_z],
                                m_skybox.ImageList[name][neg_z]);
    skybox->addChild( geode );

    osg::Group* root = new osg::Group;
    root->addChild( scene );
    root->addChild( skybox );

    view->setSceneData( root );
}

bool HoloWindow::AddOsgDockWidget(QWidget* parent)
{
    QString osgName = QString::fromLatin1("Scene View");
    OsgGridWidget *osgGrid = new OsgGridWidget(osgName, parent);
    osgGrid->setFrameStyle(QFrame::Box | QFrame::Sunken);
    osgGrid->Init(m_viewer->GetViewerArgs(), "map");
    MyDock *osgDock = new MyDock(osgName, this, Qt::WindowFlags(0), osgGrid);
    osgDock->setCustomSizeHint(m_customSizeHints.value("Scene View"));
    osgDock->setFloating(false);
    addDockWidget(Qt::RightDockWidgetArea, osgDock);
    dockWidgetMenu->addMenu(osgDock->menu);
    osgGrid->show();

    return true;
}



