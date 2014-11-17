#include "qtLib.h"
#ifndef _TRACKER_DOCK_H
#define _TRACKER_DOCK_H

#include "DockFrame.h"
#include "ITracker.h"
#include "KinectTracker.h"
#include "ICallable.h"
#include "Args.h"
#include <string>

class  TrackerFrame :  public DockFrame
{
public:

    TrackerFrame::TrackerFrame(QString name, QWidget *parent = 0) : DockFrame(name, parent)
    {
        
        RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen);
    }

    void SetTracker(KinectTracker* tracker)
    {
        pTracker = tracker;
        //if (pTracker)
        //{
        //    QWindow* hwnd  = this->windowHandle();
        //    pTracker->SetWindowHandler(static_cast<void*>(hwnd->handle()));
        //}
    }

    std::string TrackerHandlerArg;

protected:
    
    virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result)
    {
        if (pTracker)
        {
            pTracker->PaintEvent(message);
        }

        return DockFrame::nativeEvent(eventType, message, result);
    }
protected:

    KinectTracker* pTracker;
};

#endif