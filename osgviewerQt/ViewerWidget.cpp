#include "ViewerWidget.h"

#ifdef _DEBUG
#include "vld.h"
#endif

ViewerWidget::ViewerWidget(QWidget* parent, osg::ref_ptr<osgViewer::View> view, osg::ref_ptr<osg::Group> scene) : QWidget(parent), m_osgView(view), m_osgScene(scene)
{
    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    setKeyEventSetsDone(0);
}

ViewerWidget::~ViewerWidget() 
{
}

void ViewerWidget::Init()
{
    m_qtWidget = addViewWidget( m_osgView, createGraphicsWindow(0, 0, 800, 600) , m_osgScene );
    QGridLayout* grid = new QGridLayout;
    grid->addWidget( m_qtWidget, 0, 0 );
    setLayout( grid );
    RenderFlags(QWidget::DrawChildren | QWidget::IgnoreMask);
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer.start( 10 );
}

osgQt::GraphicsWindowQt* ViewerWidget::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    ds->setStereo(true);
    ds->setStereoMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
    ds->setDisplayType(osg::DisplaySettings::MONITOR);
    ds->setSplitStereoHorizontalEyeMapping(osg::DisplaySettings::LEFT_EYE_LEFT_VIEWPORT);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = name;
    traits->windowDecoration = windowDecoration;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();
  
	return new osgQt::GraphicsWindowQt(traits.get());
}

QWidget* ViewerWidget::addViewWidget(osgViewer::View *view, osgQt::GraphicsWindowQt* gw, osg::Node* scene )
{
    addView( view );

    osg::Camera* camera = view->getCamera();
    camera->setGraphicsContext( gw );
	/*osg::Matrixd viewMatrix;
	osg::Matrixd projectionMatrix;
	camera->setViewMatrix( viewMatrix );
	camera->setProjectionMatrix( projectionMatrix );
	camera->setViewport( new osg::Viewport(x, y, w, h) );*/

    const osg::GraphicsContext::Traits* traits = gw->getTraits();

    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    view->setSceneData( scene );
    view->addEventHandler( new osgViewer::StatsHandler );
    view->setCameraManipulator( new osgGA::TrackballManipulator );

    return gw->getGLWidget();
}

void ViewerWidget::paintEvent( QPaintEvent* event )
{ 
	frame(); 
}


//int main( int argc, char** argv )
//{
//    osg::ArgumentParser arguments(&argc, argv);
//
//#if QT_VERSION >= 0x050000
//    // Qt5 is currently crashing and reporting "Cannot make QOpenGLContext current in a different thread" when the viewer is run multi-threaded, this is regression from Qt4
//    osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::SingleThreaded;
//#else
//    osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
//#endif
//
//    while (arguments.read("--SingleThreaded")) threadingModel = osgViewer::ViewerBase::SingleThreaded;
//    while (arguments.read("--CullDrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
//    while (arguments.read("--DrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::DrawThreadPerContext;
//    while (arguments.read("--CullThreadPerCameraDrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext;
//
//    QApplication app(argc, argv);
//    ViewerWidget* viewWidget;
//
//	try
//    { 
//		viewWidget = new ViewerWidget(threadingModel);
//		viewWidget->setGeometry( 100, 100, 800, 600 );
//		viewWidget->show();
//        /*root = osgDB::readNodeFile("cessna.osg");
//        viewer.setSceneData(root.get()); */
//    }
//    catch(std::bad_alloc)
//    { 
//        std::cout << "a bad_alloc exception just occured"; 
//    }
//    
//    return app.exec();
//}
