#include "qtLib.h"
#ifndef TOOLBAR_H
#define TOOLBAR_H

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QLabel)

class ToolBar : public QToolBar
{
    Q_OBJECT

    QAction *m_openSceneAction;
    QAction *m_showScreenAction;
    QAction *m_runAction;
    QAction *m_moreAction;
    QAction *m_applyAction;

    QSpinBox *spinbox;
    QAction *spinboxAction;

    QAction *orderAction;
    QAction *randomizeAction;
    QAction *addSpinBoxAction;
    QAction *removeSpinBoxAction;

    QAction *movableAction;

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

    QAction *toolBarBreakAction;

public:
    ToolBar(const QString &title, QWidget *parent);

    QMenu *menu;
 
protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    virtual void mouseMoveEvent ( QMouseEvent * mouseEvent );
    static QString lastPath;
    QWidget* m_parent;
private:
    void allow(Qt::ToolBarArea area, bool allow);
    void place(Qt::ToolBarArea area, bool place);
    QLabel *tip;

private slots:
    void openScene(QString& openFilesPath = ToolBar::lastPath);
    void showScene();
    void run();
    void add();
    void apply();

    void order();
    void randomize();
    void addSpinBox();
    void removeSpinBox();

    void changeMovable(bool movable);

    void allowLeft(bool a);
    void allowRight(bool a);
    void allowTop(bool a);
    void allowBottom(bool a);

    void placeLeft(bool p);
    void placeRight(bool p);
    void placeTop(bool p);
    void placeBottom(bool p);

    void updateMenu();
    void insertToolBarBreak();

};

#endif
