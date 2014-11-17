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
    void closeEvent(QCloseEvent *event);
    void readSettings();
    QMap<QString, QSize> m_customSizeHints;
    QRect menuBarRect;
    QRect statusBarRect;
    QRect toolBarRect;
    virtual void ExtendToAllScreens(QWidget* widget);
    virtual void ReduceToScreen(QWidget* widget, int screenId);
    virtual void ToogleScreen();
    virtual void UpdateScreen();
    virtual void toogleMenuBar();
    virtual void toogleStatusBar();
    virtual void toogleToolBar();
    
public slots:
    void actionTriggered(QAction *action);
    void saveLayout();
    void loadLayout();
    void setCorner(int id);
    void switchLayoutDirection();
    void setDockOptions();
    void createDockWidget();
    void destroyDockWidget(QAction *action);
    int GetMainScreen();
    void SetMainScreen(int id);
private:
    void setupToolBar();
    void setupMenuBar();
    void setupDockWidgets(const QMap<QString, QSize> &customSizeHints);
    bool isMenuBarOn;
    bool isToolBarOn;
    bool isStatusBarOn;
    bool isExtendedToAllDisplays;
    int mainScreenId;
};


#endif
