/*#include "SelectionFrameSystem.hpp"

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include <iostream>
#include <cstdint>

int32_t main(int32_t argc, char** argv)
{
    osg::ref_ptr<osg::Node> model0 = osgDB::readNodeFile("/home/user/Models/cessna.osg");
    osg::ref_ptr<osg::Node> model1 = osgDB::readNodeFile("/home/user/Models/cow.osg");
    
    osg::ref_ptr<osg::Group> root = new osg::Group();

    root->addChild(model0.get());
    root->addChild(model1.get());

    osg::ref_ptr<SelectionFrameSystem> selectionFrameSystem = new SelectionFrameSystem();

    root->addChild(selectionFrameSystem->getSelectionFrame());

    osgViewer::Viewer viewer;

    viewer.addEventHandler(selectionFrameSystem.get());
    viewer.setUpViewInWindow(0, 0, 1920, 1080, 0);
    viewer.setSceneData(root.get());

    return viewer.run();
}*/