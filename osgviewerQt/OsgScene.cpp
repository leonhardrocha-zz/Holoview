#include "OsgScene.h"
#include <osgDB/ReadFile>

void OsgScene::AddAsset(std::string filename)
{
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(filename);
    AddModel(model);
}
