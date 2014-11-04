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
    OsgViewerWidget::OsgViewerWidget(QWidget *parent) : ViewerWidget(parent)
    {
        setMouseTracking(true);
    }

    virtual void AddView(IArgs *viewArgs = NULL, std::string viewName = "")
    {
        if (viewArgs == NULL)
        {
            return;
        }

        if (viewArgs->Exists(viewName))
        {
            osgViewer::View* view = static_cast<osgViewer::View*>(viewArgs->Get(viewName));
            if (view)
            {
                Init(view->getViewerBase(), view->getCamera());
            }
        }
    }
protected:
};

#endif