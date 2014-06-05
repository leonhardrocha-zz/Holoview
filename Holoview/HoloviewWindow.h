#pragma once
#include "IView.h"
#include "ITracker.h"
#include "OpenGLHelper.h"
#include "OpenGLWindow.h"
#include "KinectTracker.h"
#include "TrackingResults.h"
#include "ViewerWidget.h"
#include "PickHandler.h"
#include <osgViewer/CompositeViewer>

class HoloviewWindow : public OpenGLWindow
{
public:
    HoloviewWindow(QWindow *parent = (QWindow*)0, QWidget *parentView = (QWidget*)0) : OpenGLWindow(parent), m_pResults(NULL) 
    {
         m_osgPicker = new PickHandler();
         m_osgView = new osgViewer::View;
         m_osgScene = new osg::Group;
         m_osgWidget = new ViewerWidget(parentView, m_osgView, m_osgScene);
         tracker = new KinectTracker();
    };

    ~HoloviewWindow() 
    {
        if (tracker)
        {
            delete tracker;
            tracker = NULL;
        }
    };
	virtual void initialize();
    virtual void render();
	virtual void SetupWindow(int width, int height);
    virtual void ResetWindow();
	virtual void SetupScene();
	virtual void RenderScene();
	virtual void SetTrackingResults(int trackedId, TrackingResults *results);
	ITracker* GetTracker() { return static_cast<ITracker*>(tracker); };
	ViewerWidget* GetViewerWidget() { return m_osgWidget; };

protected:
	static void TrackerUpdateStatic(void* lpParam, void* args=NULL);
	//OpenGLWindow overrides
	virtual void resizeWindow();
	osg::ref_ptr<ViewerWidget> m_osgWidget;
    osg::ref_ptr<osgViewer::View> m_osgView;
    osg::ref_ptr<osg::Group> m_osgScene;
    osg::ref_ptr<PickHandler> m_osgPicker;
	KinectTracker* tracker;
	TrackingResults* m_pResults;

	Pose m_3rdPersonView;
	int windowWidth;
	int windowHeight;

};