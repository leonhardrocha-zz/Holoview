#include "osgLib.h"
#include "qtLib.h"
#ifndef HOLOVIEWHELPER_H
#define HOLOVIEWHELPER_H
#include "KinectTracker.h"
#include "DualScreenViewer.h"
#include "SkyBox.h"
#include "Grid.h"
#include <map>
#include <string>
#include <vector>



namespace HoloviewHelper
{
    void usage()
    {
        qWarning() << "Usage: mainwindow [-SizeHint<color> <width>x<height>] ...";
        exit(1);
    }

    enum SKYBOX_ID
    {
        Axis = 0,
        Snow = 1,
        NumOfSkyBoxes = 2
    };
    enum SKYBOX_SIDE
    {
        pos_x = 0,
        neg_x = 1,
        pos_y = 2,
        neg_y = 3,
        pos_z = 4,
        neg_z = 5,
        NumOfBoxSides = 6
    };
    struct SkyBoxInfo
    {
        SkyBoxInfo()
        {
            name[Snow] = "snow";
            ext[Snow] = ".jpg";
            flipX[Snow] = true;
            flipY[Snow] = true;

            name[Axis] = "axis";
            ext[Axis] = ".png";
            flipX[Axis] = false;
            flipY[Axis] = false;

            std::string sign[] = { "pos", "neg" };
            std::string axis[] = { "x", "y", "z" };

            std::vector<osg::Image*> images;
            for (int j=0; j < NumOfSkyBoxes; j++) 
            {
                std::string path = "../Dependencies/Images/Cubemap/" + name[j] + "/";
                for (int i=0; i < NumOfBoxSides; i++)
                {
                    std::string filename = path + sign[i%2] + axis[i/2] + ext[j];
                    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(filename);
                    if (flipX)
                    {
                        image->flipHorizontal();
                    }
                    if (flipY)
                    {
                        image->flipVertical();
                    }
                    ImageList[name[j]].push_back(image);
                }
            }
        };
    public:
        std::map<std::string, std::vector<osg::ref_ptr<osg::Image>>> ImageList;
    protected:
        std::string name[NumOfSkyBoxes];
        std::string ext[NumOfSkyBoxes];
        bool flipX[NumOfSkyBoxes];
        bool flipY[NumOfSkyBoxes];
    };

    SkyBoxInfo m_skybox;

    QMap<QString, QSize> parseCustomSizeHints(int argc, char **argv)
    {
        QMap<QString, QSize> result;

        for (int i = 1; i < argc; ++i) {
            QString arg = QString::fromLocal8Bit(argv[i]);

            if (arg.startsWith(QLatin1String("-SizeHint"))) {
                QString name = arg.mid(9);
                if (name.isEmpty())
                    usage();
                if (++i == argc)
                    usage();
                QString sizeStr = QString::fromLocal8Bit(argv[i]);
                int idx = sizeStr.indexOf(QLatin1Char('x'));
                if (idx == -1)
                    usage();
                bool ok;
                int w = sizeStr.left(idx).toInt(&ok);
                if (!ok)
                    usage();
                int h = sizeStr.mid(idx + 1).toInt(&ok);
                if (!ok)
                    usage();
                result[name] = QSize(w, h);
            }
        }

        return result;
    }

    static osg::Vec3 eye;

    void ViewUpdateStatic(void* lpParam)
    {
        DualScreenViewer* dualViewer = reinterpret_cast<DualScreenViewer*>(lpParam);
        if(dualViewer)
        {
            dualViewer->SetVirtualEye(eye);
            dualViewer->Update();
        }
    }

    void TrackerUpdateStatic(void* lpParam)
    {
        KinectTracker* tracker = reinterpret_cast<KinectTracker*>(lpParam);
        if(tracker)
        {
            double x,y,z;
            tracker->GetPosition(x,y,z);
            eye = osg::Vec3(x,y,z);
        }
    }

    static double ModelRadius = 0.15;
    static osg::Quat ModelAttitude;

    osg::ref_ptr<osg::PositionAttitudeTransform> GetModelTransformHelper(const osg::ref_ptr<osg::Node> model, const osg::Vec3 modelPosition)
    {
        osg::BoundingSphere bSphere = model->computeBound();
        osg::Vec3 translationToModel = -bSphere.center();
        osg::Matrix m = osg::Matrix::translate(translationToModel);
        double modelScale = ModelRadius * 1.0/bSphere.radius();
        osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
        transform->setScale(osg::Vec3(modelScale, modelScale, modelScale));
        transform->setPosition(osg::Vec3(modelPosition.x(), modelPosition.y(), modelPosition.z()));
        transform->setAttitude(ModelAttitude);
        transform->addChild(model);
        return transform;
    }

    void AddGrid(osgViewer::View* view)
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

    void AddSkyBox(osgViewer::View* view, std::string name)
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

    bool Init(DualScreenViewer& dualViewer)
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

        // View 0 -- Just the loaded model.

        osgViewer::View* mainView = dualViewer.GetMainView();
        mainView->setSceneData( scene.get() );
        AddGrid( mainView );
        AddSkyBox( mainView , "snow");

        // View 1 -- Contains the loaded model, as well as a wireframe frustum derived from View 0's Camera.

        osgViewer::View* mapView = dualViewer.GetMapView();
        root->addChild( scene.get() );
        root->addChild( dualViewer.makeFrustumFromCamera( mainView ) );
        mapView->setSceneData( root.get() );

        return true;
    }
}

#endif