#include "osgLib.h"
#ifndef _VIEWERWIDGET_H
#define _VIEWERWIDGET_H
#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <iostream>

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:
    ViewerWidget(QWidget* parent = (QWidget*)NULL);
    ~ViewerWidget();
    virtual void CreateGraphicsWindow();
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    virtual void SetStereoSettings();
    virtual void UnsetStereoSettings();

protected:
    virtual void paintEvent( QPaintEvent* event );
    QTimer _timer;
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    QWidget* m_parent;
};
#endif