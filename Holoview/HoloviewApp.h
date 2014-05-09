#pragma once
#include <QtWidgets/QApplication>
#include "Holoview.h"
#include "KinectTracker.h"
#ifdef _DEBUG
#include "vld.h"
#endif

class HoloviewApp : public QApplication
{
	Q_OBJECT
public:
	HoloviewApp(int argc, char *argv[]);
	void Usage();
	QMap<QString, QSize> GetSizeHints() { return sizeHints; };
protected:
	virtual void ParseCustomSizeHints(int argc, char **argv);
	QMap<QString, QSize> sizeHints;
	Holoview* mainWindow;
	//KinectTracker tracker;
	virtual void aboutToQuit(void);
};
