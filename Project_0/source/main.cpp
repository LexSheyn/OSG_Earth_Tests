#include <osg/Camera>
#include <osg/MatrixTransform>
#include <osg/AnimationPath>
#include <osgDB/ReadFile>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgViewer/Viewer>

#include <iostream>
#include <cstdint>

osg::AnimationPathCallback* createAnimationPathCallback(float radius, float time)
{
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
    path->setLoopMode(osg::AnimationPath::LOOP);

    unsigned int numSamples = 32;
    float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
    float delta_time = time / (float)numSamples;

    for (unsigned int i = 0; i < numSamples; i++)
    {
        float yaw = delta_yaw * (float)i;
        osg::Vec3 pos(sinf(yaw)*radius, cosf(yaw)*radius, 0.0f);
        osg::Quat rot(-yaw, osg::Z_AXIS);
        path->insert(delta_time*(float)i, osg::AnimationPath::ControlPoint(pos, rot));
    }

    osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
    apcb->setAnimationPath(path.get());
    return apcb.release();
}

int32_t main(int32_t argc, char** argv)
{
    osg::Node* model   = osgDB::readNodeFile("/home/user/Models/cessna.osg.0,0,90.rot");
    osg::Node* terrain = osgDB::readNodeFile("/home/user/Models/lz.osg");

    osg::ref_ptr<osg::MatrixTransform> modelTransform = new osg::MatrixTransform();

    modelTransform->addUpdateCallback(createAnimationPathCallback(100.0f, 20.0f));
    modelTransform->addChild(model);

    osg::ref_ptr<osg::MatrixTransform> terrainTransform = new osg::MatrixTransform();

    terrainTransform->addChild(terrain);
    terrainTransform->setMatrix(osg::Matrix::translate(0.0f, 0.0f, -200.0f));

    osg::ref_ptr<osg::Group> root = new osg::Group();

    root->addChild(modelTransform.get());
    root->addChild(terrainTransform.get());

    osg::ref_ptr<osgGA::TrackballManipulator> trackballManipulator = new osgGA::TrackballManipulator();
    osg::ref_ptr<osgGA::NodeTrackerManipulator> nodeTrackerManipulator = new osgGA::NodeTrackerManipulator();

    //nodeTrackerManipulator->setHomePosition(osg::Vec3(1000.0f, 0.0f, 0.0f), osg::Vec3(), osg::Vec3(0.0, 1.0, 0.0));
    nodeTrackerManipulator->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
    nodeTrackerManipulator->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
    nodeTrackerManipulator->setTrackNode(model);

    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keySwitch = new osgGA::KeySwitchMatrixManipulator();

    keySwitch->addMatrixManipulator('1', "Trackball", trackballManipulator.get());
    keySwitch->addMatrixManipulator('2', "NodeTracker", nodeTrackerManipulator.get());

    osgViewer::Viewer viewer;

    viewer.setUpViewInWindow(0, 0, 1920, 1080, 0);
    viewer.setCameraManipulator(keySwitch.get());
    viewer.setSceneData(root.get());

    return viewer.run();
}