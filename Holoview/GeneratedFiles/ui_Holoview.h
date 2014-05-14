/********************************************************************************
** Form generated from reading UI file 'Holoview.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOLOVIEW_H
#define UI_HOLOVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HoloviewClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *HoloviewClass)
    {
        if (HoloviewClass->objectName().isEmpty())
            HoloviewClass->setObjectName(QStringLiteral("HoloviewClass"));
        HoloviewClass->resize(600, 400);
        centralWidget = new QWidget(HoloviewClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        HoloviewClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(HoloviewClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        HoloviewClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(HoloviewClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        HoloviewClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(HoloviewClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        HoloviewClass->setStatusBar(statusBar);

        retranslateUi(HoloviewClass);

        QMetaObject::connectSlotsByName(HoloviewClass);
    } // setupUi

    void retranslateUi(QMainWindow *HoloviewClass)
    {
        HoloviewClass->setWindowTitle(QApplication::translate("HoloviewClass", "Holoview", 0));
    } // retranslateUi

};

namespace Ui {
    class HoloviewClass: public Ui_HoloviewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOLOVIEW_H
