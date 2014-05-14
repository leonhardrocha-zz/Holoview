#include "stdafx.h"
#include "Holoview.h"
#include "AssetWindow.h"
#include "MainWindow.h"
#include "ITrackerFactory.h"
#include "Factory.h"
#include "WindowsKinectFactory.h"

#include "HoloviewApp.h"
#include <QtWidgets/QApplication>

#ifdef _DEBUG
#include "vld.h"
#endif
	
void usage()
{
    qWarning() << "Usage: mainwindow [-SizeHint<color> <width>x<height>] ...";
    exit(1);
}

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

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QMap<QString, QSize> customSizeHints = parseCustomSizeHints(argc, argv);
	Holoview mainWindow(customSizeHints);
	mainWindow.menuBar()->addMenu("&File")->addAction("&Exit", &app, SLOT(quit()));		
	mainWindow.RegisterTracker(mainWindow.GetTracker());
	mainWindow.AddTrackerDockWidget();

	QWidget central(&mainWindow);	
	QWidget trackerWidget(&central);
	mainWindow.show();

	//ViewerWidget* viewWidget = new ViewerWidget();
 //   viewWidget->setGeometry( 100, 100, 800, 600 );
 //   viewWidget->show();

	app.exec();
	return 0;
}

