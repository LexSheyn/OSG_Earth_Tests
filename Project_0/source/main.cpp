#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>

#include <osgEarth/MapNode>
#include <osgEarth/EarthManipulator>
#include <osgEarth/GeoTransform>

#include <iostream>
#include <cstdint>

// TO DO

int32_t main(int32_t argc, char** argv)
{
    osg::Node* node0 = osgDB::readNodeFile("/home/user/Downloads/HYP_50M_SR_W/planet.earth");
    osg::Node* node1 = osgDB::readNodeFile("/home/user/Downloads/GRAY_50M_SR_OB/planet.earth");

    osgEarth::initialize();

    osgEarth::EarthManipulator* earthManipulator = new osgEarth::EarthManipulator();

    osgEarth::MapNode* mapNode0 = osgEarth::MapNode::get(node0);

    osgEarth::MapNode* mapNode1 = osgEarth::MapNode::get(node1);

    osgEarth::GeoTransform* geoTransform = new osgEarth::GeoTransform();

    osgEarth::GeoPoint geoPoint(mapNode0->getMapSRS(), 0, 0, 10'000'000, osgEarth::ALTMODE_RELATIVE);

    geoTransform->setPosition(geoPoint);
    geoTransform->addChild(mapNode1);

    mapNode0->addChild(geoTransform);

    osg::Group* root = new osg::Group();

    root->addChild(mapNode0);

    osgViewer::Viewer viewer;

    viewer.setCameraManipulator(earthManipulator);

    earthManipulator->setNode(mapNode1);

    osgEarth::EarthManipulator::Settings* settings = earthManipulator->getSettings();

    settings->setLockAzimuthWhilePanning(false);
    settings->setScrollSensitivity(1.5);
    settings->setThrowingEnabled(true);
    settings->setThrowDecayRate(0.02);
    settings->setMinMaxDistance(1'000'000.0, 100'000'000.0);
    settings->setArcViewpointTransitions(true);
    settings->setAutoViewpointDurationEnabled(true);
    settings->setAutoViewpointDurationLimits(0.0, 0.5);

    root->addEventCallback(new osg::NodeCallback());

    viewer.setUpViewInWindow(0, 0, 1920, 1080, 0);

    viewer.setSceneData(root);

    return viewer.run();
}