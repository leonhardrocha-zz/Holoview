#ifndef _OSG_DOCK_H
#define _OSG_DOCK_H

#include <QWidget>
#include <QEvent>
#include <QPaintEngine>
#include <QThread>
#include "DockFrame.h"
#include "ViewerWidget.h"

class  OSGFrame :  public DockFrame
{
public:

	OSGFrame::OSGFrame(const QString &dockName, QWidget *parent) : DockFrame(dockName, parent)
	{
		RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
		setAttribute(Qt::WA_NativeWindow);
		setAttribute(Qt::WA_PaintOnScreen);	
	}
	virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result)
	{	
		osgViewer.frame();
		return true;
	}

	virtual void paintEvent( QPaintEvent* e )
	{ 
		osgViewer.frame();
	}

	ViewerWidget osgViewer;
};

#endif