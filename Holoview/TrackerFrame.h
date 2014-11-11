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

    TrackerFrame::TrackerFrame(QWidget *parent, QString name) : DockFrame(name, parent)
    {
        
        RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen);
        TrackerHandlerArg = "TrackerHandlerArg";
    }

    bool Init(IArgs* args = NULL)
    {
        m_args = args;
        
        return args !=NULL;
    }

    std::string TrackerHandlerArg;

protected:
    
    virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result)
    {
        if (m_args && m_args->Exists(TrackerHandlerArg))
        {
            KinectTracker* tracker = static_cast<KinectTracker*>(m_args->Get(TrackerHandlerArg));
            if (tracker)
            {
                m_args->Set(tracker->WindowHandlerArg, message);
                tracker->PaintEvent(m_args);
            }
        } 

        return DockFrame::nativeEvent(eventType, message, result);
    }
protected:

    IArgs* m_args;
};

#endif