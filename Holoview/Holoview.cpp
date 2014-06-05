#include "stdafx.h"
#include "Holoview.h"
#include "MainWindow.h"
#include "ui_HoloviewMainForm.h"

Holoview::Holoview(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
    holoWindow.initialize();
    setCentralWidget(holoWindow.GetViewerWidget());
}

Holoview::~Holoview()
{

}


