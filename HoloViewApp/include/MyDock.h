#ifndef MYDOCK_H
#define MYDOCK_H
#include "ITracker.h"
#include "TrackerFrame.h"
#include <QAction>
#include <QFrame>
#include <QDockWidget>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)

class MyDock : public QDockWidget
{
    Q_OBJECT

    QAction *closableAction;
    QAction *movableAction;
    QAction *floatableAction;
    QAction *floatingAction;
    QAction *verticalTitleBarAction;

    QActionGroup *allowedAreasActions;
    QAction *allowLeftAction;
    QAction *allowRightAction;
    QAction *allowTopAction;
    QAction *allowBottomAction;

    QActionGroup *areaActions;
    QAction *leftAction;
    QAction *rightAction;
    QAction *topAction;
    QAction *bottomAction;

    QAction *changeSizeHintsAction;

    QMenu *tabMenu;
    QMenu *splitHMenu;
    QMenu *splitVMenu;

    QAction *windowModifiedAction;

public:
    explicit MyDock(const QString &name, QWidget *parent = 0, Qt::WindowFlags flags = 0, DockFrame *frame = NULL);
	DockFrame* m_dockFrame;
    QMenu *menu;
    void setCustomSizeHint(const QSize &size);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void resizeEvent(QResizeEvent *e);	
	virtual void paintEvent(QPaintEvent *e);

private:
    void allow(Qt::DockWidgetArea area, bool allow);
    void place(Qt::DockWidgetArea area, bool place);

private slots:
    void changeClosable(bool on);
    void changeMovable(bool on);
    void changeFloatable(bool on);
    void changeFloating(bool on);
    void changeVerticalTitleBar(bool on);
    void updateContextMenu();

    void allowLeft(bool a);
    void allowRight(bool a);
    void allowTop(bool a);
    void allowBottom(bool a);

    void placeLeft(bool p);
    void placeRight(bool p);
    void placeTop(bool p);
    void placeBottom(bool p);

    void splitInto(QAction *action);
    void tabInto(QAction *action);

};

#endif