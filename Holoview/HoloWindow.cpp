#include "stdafx.h"
#include "HoloWindow.h"

HoloWindow::HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
    osg::ref_ptr<osg::Node> duck = osgDB::readNodeFile("../Dependencies/Models/Collada/duck.dae.-90,-90,0.rot");
    osg::ref_ptr<osg::Node> airplane = osgDB::readNodeFile("../Dependencies/Models/3ds/airplane/Airplane AN-2 N200314.3DS.-90,-10,0.rot");
    osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("../Dependencies/Models/osg/cessna.osgt.-90,0,0.rot");
    osg::ref_ptr<osg::Node> kinect = osgDB::readNodeFile("../Dependencies/Models/3ds/kinect/kinect_edited.3ds.-20,0,0.rot");

    osg::Vec3 modelPosition1(0.50, 1.2, 0.0);
    osg::Vec3 modelPosition2(-0.50, 1.2, 0.0);
    osg::Vec3 modelPosition3(0.25, 1.2,  0.0);
    osg::Vec3 modelPosition4(-0.25, 1.2, 0.0);
    osg::Vec3 modelPosition5(0.0, 0.65, -0.66); //kinect

    osg::ref_ptr<osg::PositionAttitudeTransform> transform1 = GetModelTransformHelper(duck, modelPosition1);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform2 = GetModelTransformHelper(airplane, modelPosition2);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform3 = GetModelTransformHelper(cessna, modelPosition3);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform4 = GetModelTransformHelper(duck, modelPosition4);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform5 = GetModelTransformHelper(kinect, modelPosition5);

    osg::ref_ptr<osg::Group> root = new osg::Group();
    osg::ref_ptr<osg::Group> scene = new osg::Group();
    
    scene->addChild(transform1);
    scene->addChild(transform2);
    scene->addChild(transform3);
    scene->addChild(transform4);
    scene->addChild(transform5);

    root->addChild( scene.get() );
    

    // Turn on FSAA, makes the lines look better.
    osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
     m_viewer = new DualScreenViewer();
   
    //m_viewer->SetStereoSettings();
    //m_viewer->CreateGraphicsWindow();
    // Create View 0 -- Just the loaded model.
    {
        osgViewer::View* view = new osgViewer::View;
        m_viewer->addView( view );
        view->setSceneData( scene.get() );
    }

    // Create view 1 -- Contains the loaded moel, as well as a wireframe frustum derived from View 0's Camera.
    {
        osgViewer::View* view = new osgViewer::View;
        m_viewer->addView( view );
        view->setUpViewInWindow( 10, 510, 640, 480 );
        root->addChild( m_viewer->makeFrustumFromCamera() );
        view->setSceneData( root.get() );
        view->setCameraManipulator( new osgGA::TrackballManipulator );
    }

    

    osg::ref_ptr<osgGA::TrackerManipulator> trackerManipulator = new osgGA::TrackerManipulator();
    trackerManipulator->setName("Tracker");
    trackerManipulator->setVerticalAxisFixed(false);

    osg::ref_ptr<osgGA::JoystickManipulator> joystickManipulator = new osgGA::JoystickManipulator();
    joystickManipulator->setName("Joystick");

    osg::ref_ptr<osgGA::TrackballManipulator> mouseManipulator = new osgGA::TrackballManipulator;
    mouseManipulator->setName("Mouse");

    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keySwitch = new osgGA::KeySwitchMatrixManipulator;
    keySwitch->addMatrixManipulator( '1', trackerManipulator->getName(), trackerManipulator );
    keySwitch->addMatrixManipulator( '2', joystickManipulator->getName(), joystickManipulator );
    keySwitch->addMatrixManipulator( '3', mouseManipulator->getName(), mouseManipulator );

    m_viewer->getView(0)->setCameraManipulator( keySwitch );
    m_viewer->getView(1)->setCameraManipulator( mouseManipulator );

    AddSkyBox();
    AddGrid();
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

void HoloWindow::AddGrid()
{
    osg::ref_ptr<osg::Node> scene = m_viewer->getView(DualScreenViewer::Main)->getSceneData();
    
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( new Grid(osg::Vec3(0.0, 0.0, 0.0), osg::Vec2(25.0, 25.0), osg::Vec2(0.25, 0.25) ) );
    geode->setCullingActive( false );
    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform(osg::Matrix::rotate(-osg::PI_2, osg::Vec3(1,0,0)));
    transform->addChild( geode.get() );
    root->addChild( scene.get() );
    root->addChild( transform.get() );

    m_viewer->getView(DualScreenViewer::Main)->setSceneData( root.get() );
}

void HoloWindow::AddSkyBox()
{
    osg::ref_ptr<osg::Node> scene = m_viewer->getView(DualScreenViewer::Main)->getSceneData();
    
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
    bool flipX = true;
    bool flipY = true;
    for (int i=0; i < 6; i++)
    {
        std::string filename = path + sign[i%2] + axis[i/2] + ext;
        osg::Image* image = osgDB::readImageFile(filename);
        if (flipX)
        {
            image->flipHorizontal();
        }
        if (flipY)
        {
            image->flipVertical();
        }
        images.push_back(image);
    }

    skybox->setEnvironmentMap( 0, images[0], images[1], images[2], images[3], images[4], images[5]);

    skybox->addChild( geode.get() );
    
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( scene.get() );
    root->addChild( skybox.get() );
    
    // Create view 1 -- Contains the loaded model, as well as a wireframe frustum derived from View 0's Camera.
    {
        osgViewer::View* view = new osgViewer::View;
        m_viewer->addView( view );

        view->setUpViewInWindow( 10, 510, 640, 480 );
        view->setSceneData( root.get() );
        view->setCameraManipulator( new osgGA::TrackballManipulator );
    }
    m_viewer->getView(DualScreenViewer::Main)->setSceneData( root.get() );
}

bool HoloWindow::AddOsgDockWidget(QWidget* parent)
{
    QString osgName = QString::fromLatin1("Scene View");
    OsgFrame *osgFrame = new OsgFrame(osgName, parent, m_viewer->getView(0)->getScene());
    osgFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);

    MyDock *osgDock = new MyDock(osgName, this, Qt::WindowFlags(0), osgFrame);
    osgDock->setCustomSizeHint(m_customSizeHints.value("Scene View"));
    osgDock->setFloating(false);
    addDockWidget(Qt::RightDockWidgetArea, osgDock);
    dockWidgetMenu->addMenu(osgDock->menu);
    osgFrame->show();
    return true;
}



