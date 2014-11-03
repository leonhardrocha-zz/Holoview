#include "qtLib.h"
#ifndef _HOLOWINDOW_H
#define _HOLOWINDOW_H

#include "MainWindow.h"
#include "ViewerWidget.h"
#include "DualScreenViewer.h"
#include "VirtualScreenCallback.h"
#include "UpdateScreenCallback.h"
#include "SkyBox.h"
#include "TrackerManipulator.h"
#include "JoystickManipulator.h"
#include "MyDock.h"
#include "OsgGridWidget.h"
#include "SelectModelHandler.h"
#include "Grid.h"

#define defaultModelRadius 0.15

class HoloWindow : public MainWindow
{
    Q_OBJECT
    public:
        HoloWindow(const QMap<QString, QSize> &customSizeHints,
                    QWidget *parent = 0, Qt::WindowFlags flags = 0);
        HoloWindow(const HoloWindow& parent) : MainWindow(parent) {};
        ~HoloWindow();
        virtual void Init() { m_viewer->Init(); }
        virtual void Run() { m_viewer->run(); }
        static osg::ref_ptr<osg::PositionAttitudeTransform> GetModelTransformHelper(const osg::ref_ptr<osg::Node> model, 
                                                                                    const osg::Vec3 modelPosition = osg::Vec3(0,0,0), 
                                                                                    const osg::Quat modelAttitude = osg::Quat(), 
                                                                                    double modelRadius = defaultModelRadius);
        osgViewer::CompositeViewer* GetViewer() {return m_viewer;};
        bool AddOsgDockWidget(QWidget *parent = 0);
    protected:
        /*Ui_HoloWindowClass ui;*/
        virtual void AddSkyBox(osgViewer::View* view, std::string name);
        virtual void AddGrid(osgViewer::View* view);
        osg::ref_ptr<DualScreenViewer> m_viewer;
        osg::ref_ptr<SelectModelHandler> m_selectModel;

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
                std::string path = "../Dependencies/Images/Cubemap_" + name[j] + "/";
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
};

#endif // HOLOWINDOW_H
