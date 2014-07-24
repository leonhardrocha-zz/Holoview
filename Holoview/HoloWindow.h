#include "stdafx.h"
#ifndef _HOLOWINDOW_H
#define _HOLOWINDOW_H

#include "MainWindow.h"
#include "ViewerWidget.h"
#include "DualScreenViewer.h"
#include "VirtualScreenCallback.h"
#include "SkyBox.h"
#include "TrackerManipulator.h"
#include "JoystickManipulator.h"
#include "MyDock.h"
#include "OsgFrame.h"
#include "SelectModelHandler.h"
#include "Grid.h"

class HoloWindow : public MainWindow
{
	Q_OBJECT

public:
	HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent = 0, Qt::WindowFlags flags = 0);
    HoloWindow(const HoloWindow& parent) : MainWindow(parent) {};
	~HoloWindow();
    static osg::ref_ptr<osg::PositionAttitudeTransform> GetModelTransformHelper(const osg::ref_ptr<osg::Node> model, const osg::Vec3& worldPosition, const double modelRadius = 0.0);
    osgViewer::Viewer* GetViewer() {return m_viewer;};
    bool AddOSGWidget();
    void AddSkyBox();
    void AddGrid();
protected:
	/*Ui_HoloWindowClass ui;*/
    DualScreenViewer* m_viewer;
    osg::ref_ptr<SelectModelHandler> m_selectModel;
};

#endif // HOLOWINDOW_H
