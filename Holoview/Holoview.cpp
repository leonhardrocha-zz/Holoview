#include "stdafx.h"
#include "Holoview.h"
#include "MainWindow.h"
#include "ui_HoloviewMainForm.h"

Holoview::Holoview(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent, Qt::WindowFlags flags)
	: MainWindow(customSizeHints, parent, flags)
{
    osgWindow.initialize();
    auto viewer = osgWindow.GetViewer();
	setCentralWidget(viewer);
}

Holoview::~Holoview()
{

}


