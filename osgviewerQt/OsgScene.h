#pragma once
#include <osg/Group>
class OsgScene

{
public:
    OsgScene() : root(new osg::Group()) {};
    ~OsgScene() {};
    osg::ref_ptr<osg::Group> GetRoot() { return root; }
    void AddAsset( std::string filename );
protected:
    void AddModel(osg::ref_ptr<osg::Node> model) { root->addChild(model); }
    osg::ref_ptr<osg::Group> root;
};

