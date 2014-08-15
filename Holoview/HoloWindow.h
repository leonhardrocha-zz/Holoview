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
#include "OsgFrame.h"
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
        virtual void Run() { m_viewer->run(); }
        static osg::ref_ptr<osg::PositionAttitudeTransform> GetModelTransformHelper(const osg::ref_ptr<osg::Node> model, 
                                                                                    const osg::Vec3 modelPosition = osg::Vec3(0,0,0), 
                                                                                    const osg::Quat modelAttitude = osg::Quat(), 
                                                                                    double modelRadius = defaultModelRadius);
        osgViewer::CompositeViewer* GetViewer() {return m_viewer;};
        bool AddOsgDockWidget(QWidget *parent = 0);
    protected:
        /*Ui_HoloWindowClass ui;*/
        virtual void AddSkyBox(osgViewer::View* view);
        virtual void AddGrid(osgViewer::View* view);
        osg::ref_ptr<DualScreenViewer> m_viewer;
        osg::ref_ptr<SelectModelHandler> m_selectModel;
};

#endif // HOLOWINDOW_H
