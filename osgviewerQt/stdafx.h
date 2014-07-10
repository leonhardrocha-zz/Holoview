#ifdef _MSC_VER
#pragma once
#endif
#include <osgDB/ReadFile>

#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <osgQt/GraphicsWindowQt>

#include <osg/Camera>
#include <osg/CameraView>
#include <osg/Transform>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/TexGen>
#include <osg/TextureCubeMap>

#include <osgUtil/SmoothingVisitor>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/config/PanoramicSphericalDisplay>

#include <osgGA/CameraManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/StandardManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/CameraManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/OrbitManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/KeySwitchMatrixManipulator>

