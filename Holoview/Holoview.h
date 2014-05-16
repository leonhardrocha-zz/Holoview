#ifndef HOLOVIEW_H
#define HOLOVIEW_H

#include "MainWindow.h"
#include "AssetWindow.h"
#include "ui_Holoview.h"

#include "ITrackerFactory.h"
#include "Factory.h"
#include "WindowsKinectFactory.h"

class Holoview : public MainWindow
{
	Q_OBJECT

public:
	Holoview(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~Holoview();
	ITracker* GetTracker() { return window.GetTracker(); };

private:
	/*Ui_HoloviewClass ui;*/
	Ui::HoloviewClass ui;

	AssetWindow window;
	/*ViewerWidget osgWindow;*/
	QWidget *oglContainer;

};

#endif // HOLOVIEW_H
