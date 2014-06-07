#ifndef HoloWindow_H
#define HoloWindow_H

#include "MainWindow.h"
#include "ViewerWidget.h"

class HoloWindow : public MainWindow
{
	Q_OBJECT

public:
	HoloWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~HoloWindow();

private:
	/*Ui_HoloWindowClass ui;*/
    osg::ref_ptr<osgViewer::View> m_osgView;
    osg::ref_ptr<osg::Group> m_osgScene;
    osg::ref_ptr<PickHandler> m_osgPicker;
};

#endif // HoloWindow_H
