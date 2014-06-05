#pragma once
#include <QtWidgets/QApplication>
#include "Holoview.h"
#include "KinectTracker.h"
#ifdef _DEBUG
#include "vld.h"
#endif

class HoloviewApp;

static HoloviewApp* _appInstance = NULL;

class HoloviewApp : public QApplication
{
	Q_OBJECT
public:
	HoloviewApp(int argc, char *argv[]);
	void Usage();
	QMap<QString, QSize> sizeHints;
    int ArgsCount;
    char** Args;
    static void Run();
protected:
	virtual void ParseCustomSizeHints(int argc, char **argv);
	Holoview* mainWindow;
	virtual void aboutToQuit(void);
    
};
