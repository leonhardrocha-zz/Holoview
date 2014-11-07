#include "qtLib.h"
#ifndef _TRACKER_DOCK_H
#define _TRACKER_DOCK_H

#include "DockFrame.h"
#include "ITracker.h"
#include "ICallable.h"
#include "Args.h"
#include <string>

typedef std::pair<ITracker&,void*> TrackerEvent;

//deprecated
//class  TrackerFrame :  public DockFrame
//{
//public:
//
//    TrackerFrame::TrackerFrame(const QString &dockName, QWidget *parent, ITracker* tracker) : DockFrame(dockName, parent), m_pTracker(tracker)
//    {
//        RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
//        setAttribute(Qt::WA_NativeWindow);
//        setAttribute(Qt::WA_PaintOnScreen);    
//    }
//    virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result)
//    {    
//        m_pTracker->PaintEvent(message);
//        return true;
//    }
//    ITracker* m_pTracker;
//};

class  TrackerFrame :  public DockFrame
{
public:

    TrackerFrame::TrackerFrame(QWidget *parent, ITracker* tracker, std::string windowHandlerArg = "WindowHandlerArg", IArgs* args = NULL) : DockFrame("tracker", parent), m_pTracker(tracker), m_args(args), WindowHandlerArg(windowHandlerArg)
    {
        RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen);
    }

    virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result)
    {
        if (m_args)
        {
            m_args->Set(WindowHandlerArg, message);
            m_pTracker->TrackEvent(m_args);
        }
        return true;
    }
    
protected:
    std::string WindowHandlerArg;
    ITracker* m_pTracker;
    IArgs* m_args;
};

#endif