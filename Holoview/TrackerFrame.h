#ifndef _TRACKER_DOCK_H
#define _TRACKER_DOCK_H

#include <QWidget>
#include <QEvent>
#include <QVector>
#include <QPaintEngine>
#include <QThread>
#include "DockFrame.h"
#include "ITracker.h"

typedef std::pair<ITracker&,void*> TrackerEvent;

class  TrackerFrame :  public DockFrame
{
public:

	TrackerFrame::TrackerFrame(const QString &dockName, QWidget *parent, ITracker* tracker) : DockFrame(dockName, parent), m_pTracker(tracker)
	{
		RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
		setAttribute(Qt::WA_NativeWindow);
		setAttribute(Qt::WA_PaintOnScreen);	
	}
	virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result)
	{	
		m_pTracker->PaintEvent(message);
		return true;
	}
	ITracker* m_pTracker;
};

class  MultiTrackerFrame :  public DockFrame
{
public:

	MultiTrackerFrame::MultiTrackerFrame(const int trackerId, QWidget *parent, ITracker* tracker) : DockFrame("tracker", parent), m_pTracker(tracker)
	{
		RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
		setAttribute(Qt::WA_NativeWindow);
		setAttribute(Qt::WA_PaintOnScreen);
		id = trackerId;
	}
	virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result)
	{	
		m_pTracker->PaintEvent(message, &id);
		return true;
	}
	ITracker* m_pTracker;
	int id;
};

#endif