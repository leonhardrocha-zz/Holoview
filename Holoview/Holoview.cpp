#include "stdafx.h"
#include "Holoview.h"
#include "MainWindow.h"
#include "ui_HoloviewMainForm.h"

Holoview::Holoview(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
	//ui.setupUi(this);

	QSurfaceFormat format;
    format.setSamples(4);
	window.setFormat(format);
	window.resize(640, 480);	
	window.setAnimating(true);
	QWidget *oglContainer = QWidget::createWindowContainer(&window);
	oglContainer->setMinimumSize(640,480);
	oglContainer->setMaximumSize(1280,960);
	oglContainer->setFocusPolicy(Qt::TabFocus);	
	setCentralWidget(oglContainer);
}

Holoview::~Holoview()
{

}


