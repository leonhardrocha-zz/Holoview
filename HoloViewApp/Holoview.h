#ifndef HOLOVIEW_H
#define HOLOVIEW_H

#include <QtWidgets/QMainWindow>
#include "ui_Holoview.h"

class Holoview : public QMainWindow
{
	Q_OBJECT

public:
	Holoview(QWidget *parent = 0);
	~Holoview();

private:
	Ui::HoloviewClass ui;
};

#endif // HOLOVIEW_H
