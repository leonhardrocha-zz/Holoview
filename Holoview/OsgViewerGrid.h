#include "qtLib.h"
#ifndef _OSG_VIEWERWIDGET_H
#define _OSG_VIEWERWIDGET_H

#include "DockFrame.h"
#include "OsgViewerWidget.h"
#include "IArgs.h"

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <iostream>


class  OsgViewerGrid :  public DockFrame
{
public:
    OsgViewerGrid::OsgViewerGrid(const QString &dockName, QWidget *parent) : DockFrame(dockName, parent), MaxNumOfColumns(2), MaxNumOfRows(2)
    {
        m_grid = new QGridLayout;
        RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen);
        setLayout( m_grid );
        m_column = m_row = 0;
    }

    virtual void OsgViewerGrid::AddView(IArgs *viewArgs = NULL, std::string viewName = "")
    {
        OsgViewerWidget* viewer = new OsgViewerWidget(this);
        viewer->setMouseTracking(true);
        viewer->AddView(viewArgs, viewName);
        viewer->CreateGraphicsWindow();
        
        QWidget* viewWidget = addViewWidget(viewer->GetCamera());
        m_grid->addWidget( viewWidget, m_column, m_row );
        NextSlot();
    }

    int MaxNumOfColumns, MaxNumOfRows;

protected:

    void NextSlot()
    {
        m_column++;
        if (m_column > MaxNumOfColumns)
        {
            m_column = 0;
            m_row++;
            if (m_row > MaxNumOfRows)
            {
                m_row = 0;
            }
        }
    }

    QWidget* addViewWidget( osg::Camera* camera)
    {
        osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );
        return gw ? gw->getGraphWidget() : NULL;
    }

    QGridLayout* m_grid;
    int m_column, m_row;
    
};

#endif