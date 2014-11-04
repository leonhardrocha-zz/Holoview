#include "osgLib.h"
#ifndef _VIEWERWIDGET_H
#define _VIEWERWIDGET_H
#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <iostream>

class ViewerWidget : public QWidget/*, public osgViewer::ViewerBase*/
{
public:
    ViewerWidget(QWidget* parent = (QWidget*)NULL) : QWidget(parent) { installEventFilter(this); };
    ~ViewerWidget() {};
    virtual void Init(osgViewer::ViewerBase* view = new osgViewer::Viewer(), osg::Camera* camera = NULL);
    virtual void CreateGraphicsWindow();
    osg::ref_ptr<osg::DisplaySettings> GetDisplaySettings() { return m_displaySettings; };
    osg::ref_ptr<osg::GraphicsContext::Traits> GetTraits() { return m_traits; };
    virtual void SetStereoSettings();
    virtual void UnsetStereoSettings();
    virtual osg::Camera* GetCamera() { return m_camera.get(); }

protected:
    osg::Camera* CreateDefaultCamera(osg::GraphicsContext::Traits* traits);
    osg::GraphicsContext::Traits* CreateDefaultTraits(osg::DisplaySettings* ds, const std::string& windowName = "");
    virtual void paintEvent( QPaintEvent* event );
    bool eventFilter(QObject *o, QEvent *e);
    QTimer _timer;
    osg::ref_ptr<osg::DisplaySettings> m_displaySettings;
    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    QWidget* m_parent;
    osg::ref_ptr<osgViewer::ViewerBase> m_viewer;
    osg::ref_ptr<osg::Camera> m_camera;
};
#endif