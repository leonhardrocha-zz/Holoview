#pragma once
#include <osg/Group>
class OsgScene : osg::Group

{
public:
    OsgScene() {};
    ~OsgScene() {};
    osg::Group* GetScene() { return &m_scene; }
    void AddAsset( std::string filename );
    void AddNode(osg::ref_ptr<osg::Node> node) { m_scene.addChild(node); }
protected:
    osg::Group m_scene;
};

