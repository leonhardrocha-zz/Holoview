#pragma once
#include "IView.h"
#include "ITracker.h"
#include "OpenGLHelper.h"
#include "OpenGLWindow.h"
#include "KinectTracker.h"
#include "TrackingResults.h"
#include "ViewerWidget.h"
#include "PickHandler.h"


class HoloviewWindow : public QWindow
{
public:
    HoloviewWindow(QWindow *parent = (QWindow*)0) : QWindow(parent), m_pResults(NULL) 
    {
         m_osgPicker = new PickHandler();
         m_osgView = new osgViewer::View;
         m_osgScene = new osg::Group;
    };

    ~HoloviewWindow() 
    {
        if (m_tracker)
        {
            delete m_tracker;
            m_tracker = NULL;
        }
    };
    virtual void initialize();
    virtual void render();
    virtual void SetupWindow(int width, int height);
    virtual void ResetWindow();
    virtual void SetupScene();
    virtual void RenderScene();
    virtual void SetTrackingResults(TrackingResults *results) { m_pResults = results; };
    ITracker* GetTracker() { return static_cast<ITracker*>(m_tracker); };
    void SetTracker(ITracker* tracker) { m_tracker = tracker; };

protected:
    static void TrackerUpdateStatic(void* lpParam, TrackingArgs args=NULL);
    virtual void resizeWindow();
    osg::ref_ptr<osgViewer::View> m_osgView;
    osg::ref_ptr<osg::Group> m_osgScene;
    osg::ref_ptr<PickHandler> m_osgPicker;
    ITracker* m_tracker;
    TrackingResults* m_pResults;
    int windowWidth;
    int windowHeight;

};