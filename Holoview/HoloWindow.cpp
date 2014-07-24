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

    m_viewer = new DualScreenViewer();
    m_viewer->addEventHandler( new SelectModelHandler );
    m_viewer->SetStereoSettings();
    m_viewer->CreateGraphicsWindow();

    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keySwitch = new osgGA::KeySwitchMatrixManipulator;
    osg::ref_ptr<osgGA::TrackerManipulator> trackerManipulator = new osgGA::TrackerManipulator();
    trackerManipulator->setName("Tracker");
    osg::ref_ptr<osgGA::JoystickManipulator> joystickManipulator = new osgGA::JoystickManipulator();
    joystickManipulator->setName("Joystick");
    keySwitch->addMatrixManipulator( '1', trackerManipulator->getName(), trackerManipulator );
    keySwitch->addMatrixManipulator( '2', joystickManipulator->getName(), joystickManipulator );

    trackerManipulator->setVerticalAxisFixed(false);
    m_viewer->setCameraManipulator( keySwitch );
    
    osg::Vec3 modelPosition1(0.75, 1.2, 0.75);
    osg::Vec3 modelPosition2(-0.75, 1.2, 0.75);
    osg::Vec3 modelPosition3(0.50, 1.2,  0.50);
    osg::Vec3 modelPosition4(-0.50, 1.2, 0.50);
    osg::Vec3 modelPosition5(0.0, 0.1, 0.1); //kinect

    const osg::Vec3 screenOffset(0,0,0.76);

    osg::ref_ptr<osg::PositionAttitudeTransform> transform1 = GetModelTransformHelper(duck, 0.15);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform2 = GetModelTransformHelper(airplane,  0.15);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform3 = GetModelTransformHelper(cessna, 0.15);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform4 = GetModelTransformHelper(duck, 0.15);
    osg::ref_ptr<osg::PositionAttitudeTransform> transform5 = GetModelTransformHelper(kinect);

    osg::ref_ptr<osg::Group> root = new osg::Group();

    root->addChild(transform1);
    root->addChild(transform2);
    root->addChild(transform3);
    root->addChild(transform4);
    root->addChild(transform5);

    root->addCullCallback( new VirtualScreenCallback(transform1.get(), modelPosition1, screenOffset, m_viewer) );
    root->addCullCallback( new VirtualScreenCallback(transform2.get(), modelPosition2, screenOffset, m_viewer) );
    root->addCullCallback( new VirtualScreenCallback(transform3.get(), modelPosition3, screenOffset, m_viewer) );
    root->addCullCallback( new VirtualScreenCallback(transform4.get(), modelPosition4, screenOffset, m_viewer) );
    root->addCullCallback( new VirtualScreenCallback(transform5.get(), modelPosition5, screenOffset, m_viewer) );

    m_viewer->setSceneData(root);

    AddSkyBox();
    AddGrid();
}

HoloWindow::~HoloWindow()
{

}

osg::ref_ptr<osg::PositionAttitudeTransform> HoloWindow::GetModelTransformHelper(const osg::ref_ptr<osg::Node> model, const double modelRadius)
{
    osg::BoundingSphere bSphere = model->computeBound();
    osg::Vec3 translationToModel = -bSphere.center();
    osg::Matrix m = osg::Matrix::translate(translationToModel);
    double modelScale = modelRadius * 1.0/bSphere.radius();
    osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
    transform->setScale(osg::Vec3(modelScale, modelScale, modelScale));
    transform->addChild(model);
    return transform;
}

void HoloWindow::AddGrid()
{
    osg::ref_ptr<osg::Node> scene = m_viewer->getSceneData();
    
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( new Grid(osg::Vec3(0.0, 0.0, 0.0), osg::Vec2(25.0, 25.0), osg::Vec2(0.25, 0.25) ) );
    geode->setCullingActive( false );
    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform(osg::Matrix::rotate(-osg::PI_2, osg::Vec3(1,0,0)) * osg::Matrix::translate(osg::Vec3(0,1.0,0)));
    transform->addChild( geode.get() );
    root->addChild( scene.get() );
    root->addChild( transform.get() );

    m_viewer->setSceneData( root.get() );
}

void HoloWindow::AddSkyBox()
{
    osg::ref_ptr<osg::Node> scene = m_viewer->getSceneData();
    
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), scene->getBound().radius())) );
    geode->setCullingActive( false );
    
    osg::ref_ptr<SkyBox> skybox = new SkyBox;
    skybox->getOrCreateStateSet()->setTextureAttributeAndModes( 0, new osg::TexGen );
    std::string name = "snow";
    std::string path = "../Dependencies/Images/Cubemap_" + name + "/";
    std::string ext = ".jpg";
    std::string sign[] = { "pos", "neg" };
    std::string axis[] = { "x", "y", "z" };
    std::vector<osg::Image*> images;
    bool flipX = false;
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

    m_viewer->setSceneData( root.get() );
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


