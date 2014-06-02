#include "stdafx.h"
#include "Holoview.h"
#include "MainWindow.h"
#include "ui_HoloviewMainForm.h"

Holoview::Holoview(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
	//ui.setupUi(this);
    viewWidget.setGeometry( 100, 100, 800, 600 );
    //viewWidget.show();
	setCentralWidget(&viewWidget);
}

Holoview::~Holoview()
{

}


