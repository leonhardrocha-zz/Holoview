//#pragma once
//#include <QTimer>
//#include <QApplication>
//#include <QGridLayout>
//#include <osgViewer/CompositeViewer>
//#include <osgViewer/ViewerEventHandlers>
//#include <osgGA/TrackballManipulator>
//#include <osgDB/ReadFile>
//#include <osgQt/GraphicsWindowQt>
//#include <iostream>
//
//class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
//{
//public:
//    ViewerWidget();
//    QWidget* addViewWidget( osgQt::GraphicsWindowQt* gw, osg::Node* scene );
//    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );
//    virtual bool nativeEvent(const QByteArray& eventType, void * message, long *result);
//
//protected:
//
//    QTimer _timer;
//};
