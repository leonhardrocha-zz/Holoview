#include "stdafx.h"
#include "HoloviewApp.h"


HoloviewApp::HoloviewApp(int argc, char* argv[]) : QApplication(argc, argv) 
{
	ParseCustomSizeHints(argc,argv);
	mainWindow = new Holoview(sizeHints);
	mainWindow->menuBar()->addMenu("&File")->addAction("&Exit", this, SLOT(quit()));
	//mainWindow->RegisterTracker(window.GetTracker());
	mainWindow->AddTrackerDockWidget();

	QWidget central(mainWindow);	
	QWidget trackerWidget(&central);
	mainWindow->show();
}

void HoloviewApp::Usage()
{
    qWarning() << "Usage: mainwindow [-SizeHint<color> <width>x<height>] ...";
}


void HoloviewApp::aboutToQuit(void)
{
	delete mainWindow;
}
void HoloviewApp::ParseCustomSizeHints(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        QString arg = QString::fromLocal8Bit(argv[i]);

        if (arg.startsWith(QLatin1String("-SizeHint"))) {
            QString name = arg.mid(9);
            if (name.isEmpty())
                Usage();
            if (++i == argc)
                Usage();
            QString sizeStr = QString::fromLocal8Bit(argv[i]);
            int idx = sizeStr.indexOf(QLatin1Char('x'));
            if (idx == -1)
                Usage();
            bool ok;
            int w = sizeStr.left(idx).toInt(&ok);
            if (!ok)
                Usage();
            int h = sizeStr.mid(idx + 1).toInt(&ok);
            if (!ok)
                Usage();
            sizeHints[name] = QSize(w, h);
        }
    }
}