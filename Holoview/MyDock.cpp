#include "stdafx.h"
#include "MyDock.h"
#include <QAction>
#include <QtEvents>
#include <QFrame>
#include <QMainWindow>
#include <QMenu>
#include <QPainter>
#include <QImage>
#include <QColor>
#include <QDialog>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPainterPath>
#include <QPushButton>
#include <QHBoxLayout>
#include <QBitmap>
#include <QtDebug>

MyDock::MyDock(const QString &name, QWidget *parent, Qt::WindowFlags flags, QWidget *frame)
    : QDockWidget(parent, flags), m_dockFrame(frame)
{
    setObjectName(name + QLatin1String(" Dock Widget"));
    setWindowTitle(objectName() + QLatin1String(" [*]"));
	
	if (m_dockFrame)
	{
		setWidget(m_dockFrame);
	}

    changeSizeHintsAction = new QAction(tr("Change Size Hints"), this);
    connect(changeSizeHintsAction, SIGNAL(triggered()), frame, SLOT(changeSizeHints()));

    closableAction = new QAction(tr("Closable"), this);
    closableAction->setCheckable(true);
    connect(closableAction, SIGNAL(triggered(bool)), SLOT(changeClosable(bool)));

    movableAction = new QAction(tr("Movable"), this);
    movableAction->setCheckable(true);
    connect(movableAction, SIGNAL(triggered(bool)), SLOT(changeMovable(bool)));

    floatableAction = new QAction(tr("Floatable"), this);
    floatableAction->setCheckable(true);
    connect(floatableAction, SIGNAL(triggered(bool)), SLOT(changeFloatable(bool)));

    verticalTitleBarAction = new QAction(tr("Vertical title bar"), this);
    verticalTitleBarAction->setCheckable(true);
    connect(verticalTitleBarAction, SIGNAL(triggered(bool)),
            SLOT(changeVerticalTitleBar(bool)));

    floatingAction = new QAction(tr("Floating"), this);
    floatingAction->setCheckable(true);
    connect(floatingAction, SIGNAL(triggered(bool)), SLOT(changeFloating(bool)));

    allowedAreasActions = new QActionGroup(this);
    allowedAreasActions->setExclusive(false);

    allowLeftAction = new QAction(tr("Allow on Left"), this);
    allowLeftAction->setCheckable(true);
    connect(allowLeftAction, SIGNAL(triggered(bool)), SLOT(allowLeft(bool)));

    allowRightAction = new QAction(tr("Allow on Right"), this);
    allowRightAction->setCheckable(true);
    connect(allowRightAction, SIGNAL(triggered(bool)), SLOT(allowRight(bool)));

    allowTopAction = new QAction(tr("Allow on Top"), this);
    allowTopAction->setCheckable(true);
    connect(allowTopAction, SIGNAL(triggered(bool)), SLOT(allowTop(bool)));

    allowBottomAction = new QAction(tr("Allow on Bottom"), this);
    allowBottomAction->setCheckable(true);
    connect(allowBottomAction, SIGNAL(triggered(bool)), SLOT(allowBottom(bool)));

    allowedAreasActions->addAction(allowLeftAction);
    allowedAreasActions->addAction(allowRightAction);
    allowedAreasActions->addAction(allowTopAction);
    allowedAreasActions->addAction(allowBottomAction);

    areaActions = new QActionGroup(this);
    areaActions->setExclusive(true);

    leftAction = new QAction(tr("Place on Left") , this);
    leftAction->setCheckable(true);
    connect(leftAction, SIGNAL(triggered(bool)), SLOT(placeLeft(bool)));

    rightAction = new QAction(tr("Place on Right") , this);
    rightAction->setCheckable(true);
    connect(rightAction, SIGNAL(triggered(bool)), SLOT(placeRight(bool)));

    topAction = new QAction(tr("Place on Top") , this);
    topAction->setCheckable(true);
    connect(topAction, SIGNAL(triggered(bool)), SLOT(placeTop(bool)));

    bottomAction = new QAction(tr("Place on Bottom") , this);
    bottomAction->setCheckable(true);
    connect(bottomAction, SIGNAL(triggered(bool)), SLOT(placeBottom(bool)));

    areaActions->addAction(leftAction);
    areaActions->addAction(rightAction);
    areaActions->addAction(topAction);
    areaActions->addAction(bottomAction);

    connect(movableAction, SIGNAL(triggered(bool)), areaActions, SLOT(setEnabled(bool)));

    connect(movableAction, SIGNAL(triggered(bool)), allowedAreasActions, SLOT(setEnabled(bool)));

    connect(floatableAction, SIGNAL(triggered(bool)), floatingAction, SLOT(setEnabled(bool)));

    connect(floatingAction, SIGNAL(triggered(bool)), floatableAction, SLOT(setDisabled(bool)));
    connect(movableAction, SIGNAL(triggered(bool)), floatableAction, SLOT(setEnabled(bool)));

    tabMenu = new QMenu(this);
    tabMenu->setTitle(tr("Tab into"));
    connect(tabMenu, SIGNAL(triggered(QAction*)), this, SLOT(tabInto(QAction*)));

    splitHMenu = new QMenu(this);
    splitHMenu->setTitle(tr("Split horizontally into"));
    connect(splitHMenu, SIGNAL(triggered(QAction*)), this, SLOT(splitInto(QAction*)));

    splitVMenu = new QMenu(this);
    splitVMenu->setTitle(tr("Split vertically into"));
    connect(splitVMenu, SIGNAL(triggered(QAction*)), this, SLOT(splitInto(QAction*)));

    windowModifiedAction = new QAction(tr("Modified"), this);
    windowModifiedAction->setCheckable(true);
    windowModifiedAction->setChecked(false);
    connect(windowModifiedAction, SIGNAL(toggled(bool)), this, SLOT(setWindowModified(bool)));

    menu = new QMenu(name, this);
    menu->addAction(toggleViewAction());
    QAction *action = menu->addAction(tr("Raise"));
    connect(action, SIGNAL(triggered()), this, SLOT(raise()));
    menu->addAction(changeSizeHintsAction);
    menu->addSeparator();
    menu->addAction(closableAction);
    menu->addAction(movableAction);
    menu->addAction(floatableAction);
    menu->addAction(floatingAction);
    menu->addAction(verticalTitleBarAction);
    menu->addSeparator();
    menu->addActions(allowedAreasActions->actions());
    menu->addSeparator();
    menu->addActions(areaActions->actions());
    menu->addSeparator();
    menu->addMenu(splitHMenu);
    menu->addMenu(splitVMenu);
    menu->addMenu(tabMenu);
    menu->addSeparator();
    menu->addAction(windowModifiedAction);

    connect(menu, SIGNAL(aboutToShow()), this, SLOT(updateContextMenu()));

    leftAction->setShortcut(Qt::CTRL|Qt::Key_W);
    rightAction->setShortcut(Qt::CTRL|Qt::Key_E);
    toggleViewAction()->setShortcut(Qt::CTRL|Qt::Key_R);

}

void MyDock::updateContextMenu()
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
    const Qt::DockWidgetAreas areas = allowedAreas();

    closableAction->setChecked(features() & QDockWidget::DockWidgetClosable);
    if (windowType() == Qt::Drawer) {
        floatableAction->setEnabled(false);
        floatingAction->setEnabled(false);
        movableAction->setEnabled(false);
        verticalTitleBarAction->setChecked(false);
    } else {
        floatableAction->setChecked(features() & QDockWidget::DockWidgetFloatable);
        floatingAction->setChecked(isWindow());
        // done after floating, to get 'floatable' correctly initialized
        movableAction->setChecked(features() & QDockWidget::DockWidgetMovable);
        verticalTitleBarAction
            ->setChecked(features() & QDockWidget::DockWidgetVerticalTitleBar);
    }

    allowLeftAction->setChecked(isAreaAllowed(Qt::LeftDockWidgetArea));
    allowRightAction->setChecked(isAreaAllowed(Qt::RightDockWidgetArea));
    allowTopAction->setChecked(isAreaAllowed(Qt::TopDockWidgetArea));
    allowBottomAction->setChecked(isAreaAllowed(Qt::BottomDockWidgetArea));

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftDockWidgetArea);
        allowRightAction->setEnabled(area != Qt::RightDockWidgetArea);
        allowTopAction->setEnabled(area != Qt::TopDockWidgetArea);
        allowBottomAction->setEnabled(area != Qt::BottomDockWidgetArea);
    }

    leftAction->blockSignals(true);
    rightAction->blockSignals(true);
    topAction->blockSignals(true);
    bottomAction->blockSignals(true);

    leftAction->setChecked(area == Qt::LeftDockWidgetArea);
    rightAction->setChecked(area == Qt::RightDockWidgetArea);
    topAction->setChecked(area == Qt::TopDockWidgetArea);
    bottomAction->setChecked(area == Qt::BottomDockWidgetArea);

    leftAction->blockSignals(false);
    rightAction->blockSignals(false);
    topAction->blockSignals(false);
    bottomAction->blockSignals(false);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftDockWidgetArea);
        rightAction->setEnabled(areas & Qt::RightDockWidgetArea);
        topAction->setEnabled(areas & Qt::TopDockWidgetArea);
        bottomAction->setEnabled(areas & Qt::BottomDockWidgetArea);
    }

    tabMenu->clear();
    splitHMenu->clear();
    splitVMenu->clear();
    QList<MyDock*> dock_list = mainWindow->findChildren<MyDock*>();
    foreach (MyDock *dock, dock_list) {
//        if (!dock->isVisible() || dock->isFloating())
//            continue;
        tabMenu->addAction(dock->objectName());
        splitHMenu->addAction(dock->objectName());
        splitVMenu->addAction(dock->objectName());
    }
}

void MyDock::splitInto(QAction *action)
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    QList<MyDock*> dock_list = mainWindow->findChildren<MyDock*>();
    MyDock *target = 0;
    foreach (MyDock *dock, dock_list) {
        if (action->text() == dock->objectName()) {
            target = dock;
            break;
        }
    }
    if (target == 0)
        return;

    Qt::Orientation o = action->parent() == splitHMenu
                        ? Qt::Horizontal : Qt::Vertical;
    mainWindow->splitDockWidget(target, this, o);
}

void MyDock::tabInto(QAction *action)
{
    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    QList<MyDock*> dock_list = mainWindow->findChildren<MyDock*>();
    MyDock *target = 0;
    foreach (MyDock *dock, dock_list) {
        if (action->text() == dock->objectName()) {
            target = dock;
            break;
        }
    }
    if (target == 0)
        return;

    mainWindow->tabifyDockWidget(target, this);
}

void MyDock::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    menu->exec(event->globalPos());
}

void MyDock::resizeEvent(QResizeEvent *e)
{
    QDockWidget::resizeEvent(e);
}

void MyDock::paintEvent(QPaintEvent *e)
{
    QDockWidget::paintEvent(e);
}


void MyDock::allow(Qt::DockWidgetArea area, bool a)
{
    Qt::DockWidgetAreas areas = allowedAreas();
    areas = a ? areas | area : areas & ~area;
    setAllowedAreas(areas);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftDockWidgetArea);
        rightAction->setEnabled(areas & Qt::RightDockWidgetArea);
        topAction->setEnabled(areas & Qt::TopDockWidgetArea);
        bottomAction->setEnabled(areas & Qt::BottomDockWidgetArea);
    }
}

void MyDock::place(Qt::DockWidgetArea area, bool p)
{
    if (!p) return;

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    mainWindow->addDockWidget(area, this);

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftDockWidgetArea);
        allowRightAction->setEnabled(area != Qt::RightDockWidgetArea);
        allowTopAction->setEnabled(area != Qt::TopDockWidgetArea);
        allowBottomAction->setEnabled(area != Qt::BottomDockWidgetArea);
    }
}

void MyDock::setCustomSizeHint(const QSize &size)
{
    if (MyDock *dock = qobject_cast<MyDock*>(widget()))
        dock->setCustomSizeHint(size);
}

void MyDock::changeClosable(bool on)
{ setFeatures(on ? features() | DockWidgetClosable : features() & ~DockWidgetClosable); }

void MyDock::changeMovable(bool on)
{ setFeatures(on ? features() | DockWidgetMovable : features() & ~DockWidgetMovable); }

void MyDock::changeFloatable(bool on)
{ setFeatures(on ? features() | DockWidgetFloatable : features() & ~DockWidgetFloatable); }

void MyDock::changeFloating(bool floating)
{ setFloating(floating); }

void MyDock::allowLeft(bool a)
{ allow(Qt::LeftDockWidgetArea, a); }

void MyDock::allowRight(bool a)
{ allow(Qt::RightDockWidgetArea, a); }

void MyDock::allowTop(bool a)
{ allow(Qt::TopDockWidgetArea, a); }

void MyDock::allowBottom(bool a)
{ allow(Qt::BottomDockWidgetArea, a); }

void MyDock::placeLeft(bool p)
{ place(Qt::LeftDockWidgetArea, p); }

void MyDock::placeRight(bool p)
{ place(Qt::RightDockWidgetArea, p); }

void MyDock::placeTop(bool p)
{ place(Qt::TopDockWidgetArea, p); }

void MyDock::placeBottom(bool p)
{ place(Qt::BottomDockWidgetArea, p); }

void MyDock::changeVerticalTitleBar(bool on)
{
    setFeatures(on ? features() | DockWidgetVerticalTitleBar
                    : features() & ~DockWidgetVerticalTitleBar);
}