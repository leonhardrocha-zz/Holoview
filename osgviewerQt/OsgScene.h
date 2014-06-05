#pragma once
#include <osg/Group>
#include "PickHandler.h"
class OsgScene : osg::Group

{
public:
    OsgScene() {};
    ~OsgScene() {};
    osg::Group* GetScene() { return &m_scene; }
    void AddAsset( std::string filename );
    void AddPickNode(osg::ref_ptr<PickHandler> picker ) { m_scene.addChild(picker->getOrCreateSelectionBox()); };
    void AddNode(osg::ref_ptr<osg::Node> node) { m_scene.addChild(node); }
protected:
    osg::Group m_scene;
};

