#include "qtLib.h"
#ifndef _OSG_DOCK_H
#define _OSG_DOCK_H

#include "DockFrame.h"
#include "ViewerWidget.h"
#include "IArgs.h"
#include "ViewerWidget.h"


class  OsgViewerWidget :  public ViewerWidget
{
public:
    OsgViewerWidget::OsgViewerWidget(const QString &dockName, QWidget *parent) : ViewerWidget(parent)
    {
        setMouseTracking(true);
    }

protected:
    virtual void paintEvent(QPaintEvent *paintevent)
    { 
        bool handled = QWidget::event( paintevent );

        switch( paintevent->type() )
        {
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            this->update();
        break;

        default:
            break;
        }
    }
    QTimer _timer;
};

#endif