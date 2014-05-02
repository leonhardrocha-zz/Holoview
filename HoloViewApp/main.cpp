#include "stdafx.h"
#include "Holoview.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Holoview w;
	w.show();
	return a.exec();
}
