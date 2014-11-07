#include "qtLib.h"
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "MyDock.h"
#include "TrackerFrame.h"
#include "toolbar.h"
#include "ITracker.h"
#include "DockFrame.h"
#include "KinectTracker.h"

class ToolBar;
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSignalMapper)

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    QTextEdit *center;
    QList<ToolBar*> toolBars;
    QMenu *dockWidgetMenu;
    QMenu *mainWindowMenu;
    QSignalMapper *mapper;
    QList<QDockWidget*> extraDockWidgets;
    QAction *createDockWidgetAction;
    QMenu *destroyDockWidgetMenu;

public:
    MainWindow(const QMap<QString, QSize> &customSizeHints,
                QWidget *parent = 0, Qt::WindowFlags flags = 0);
    MainWindow(const MainWindow& parent)  { };
    bool AddTrackerDockWidget(ITracker* tracker, IArgs* args=NULL);
    bool AddViewerWidget(QWidget* parent);
    virtual void Run() {};
protected:
    void showEvent(QShowEvent *event);
    void mouseMoveEvent ( QMouseEvent * event );
    void mouseDoubleClickEvent(QMouseEvent *e);
    QMap<QString, QSize> m_customSizeHints;
    QRect menuBarRect;
    QRect statusBarRect;
public slots:
    void handleResults(const QString &);
    void actionTriggered(QAction *action);
    void saveLayout();
    void loadLayout();
    void setCorner(int id);
    void switchLayoutDirection();
    void setDockOptions();
    void createDockWidget();
    void destroyDockWidget(QAction *action);
    void ExtendToFullScreen(QWidget* widget);
signals:
    void operate(const QString &);
private:
    void setupToolBar();
    void setupMenuBar();
    void setupDockWidgets(const QMap<QString, QSize> &customSizeHints);
    bool isMenuBarOn;
    bool isStatusBarOn;
};


#endif
