#ifndef HoloWindow_H
#define HoloWindow_H

#include "MainWindow.h"
#include "ViewerWidget.h"
#include "MultiViewerWidget.h"
#include "MyDock.h"
#include "OsgFrame.h"

class HoloWindow : public MainWindow
{
	Q_OBJECT

public:
	HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~HoloWindow();
    bool AddOSGWidget();
protected:
	/*Ui_HoloWindowClass ui;*/
    std::vector<osgViewer::View*> m_views;
    osg::ref_ptr<PickHandler> m_picker;
};

#endif // HoloWindow_H
