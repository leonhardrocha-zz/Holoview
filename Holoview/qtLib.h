#ifdef _MSC_VER
#pragma once

#include <WinSock2.h>
#include <windows.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#undef _QTLIB_H
#endif

#ifndef _QTLIB_H
#define _QTLIB_H
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QColor>
#include <QComboBox>
#include <QDataStream>
#include <qdebug.h>
#include <QDialog>
#include <QDockWidget>
#include <QDockWidget>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPaintEngine>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QStatusBar>
#include <QtDebug>
#include <QtEvents>
#include <QTextEdit>
#include <QThread>
#include <QToolBar>
#include <QtWidgets>
#include <QVector>
#include <QWidget>
#endif