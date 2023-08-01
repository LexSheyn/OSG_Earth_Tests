#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <osgGA/NodeTrackerManipulator>

#include <osgEarth/MapNode>
#include <osgEarth/EarthManipulator>
#include <osgEarth/GeoTransform>

#include <iostream>
#include <cstdint>

class ManipulatorController : public osgGA::GUIEventHandler
{
public:

    ManipulatorController()
    {}

    void setViewer(osgViewer::Viewer* viewer)
    {
        m_viewer = viewer;
    }

    void setManipulator0(osgEarth::EarthManipulator* manipulator)
    {
        m_manipulator0 = manipulator;
    }

    void setManipulator1(osgEarth::EarthManipulator* manipulator)
    {
        m_manipulator1 = manipulator;
    }

    void setManipulator2(osgGA::NodeTrackerManipulator* manipulator)
    {
        m_manipulator2 = manipulator;
    }

    // osgGA::GUIEventHandler.

    virtual bool handle(const osgGA::GUIEventAdapter& eventAdapter, osgGA::GUIActionAdapter& actionAdapter) override
    {
        const osgGA::GUIEventAdapter::EventType eventType = eventAdapter.getEventType();

        if (eventType == osgGA::GUIEventAdapter::KEYUP)
        {
            const int32_t key = eventAdapter.getKey();

            switch (key)
            {
                case 'q':
                case 'Q':
                {
                    m_viewer->setCameraManipulator(m_manipulator0.get());

                    break;
                }

                case 'w':
                case 'W':
                {
                    m_viewer->setCameraManipulator(m_manipulator1.get());

                    break;
                }

                case 'e':
                case 'E':
                {
                    m_viewer->setCameraManipulator(m_manipulator2.get());

                    break;
                }

                default: break;
            }
        }

        return false;
    }

protected:

    virtual ~ManipulatorController() override
    {}

    osgViewer::Viewer*                          m_viewer;
    osg::ref_ptr<osgEarth::EarthManipulator>    m_manipulator0;
    osg::ref_ptr<osgEarth::EarthManipulator>    m_manipulator1;
    osg::ref_ptr<osgGA::NodeTrackerManipulator> m_manipulator2;
};

int32_t main(int32_t argc, char** argv)
{
    osg::Node* node0 = osgDB::readNodeFile("/home/user/Downloads/HYP_50M_SR_W/planet.earth");
    osg::Node* node1 = osgDB::readNodeFile("/home/user/Downloads/GRAY_50M_SR_OB/planet.earth");
    osg::Node* node2 = osgDB::readNodeFile("/home/user/Models/cessna.osg");

    osgEarth::initialize();

    osgEarth::MapNode* mapNode0 = osgEarth::MapNode::get(node0);
    osgEarth::MapNode* mapNode1 = osgEarth::MapNode::get(node1);

    osg::MatrixTransform* transform0 = new osg::MatrixTransform();

    transform0->setMatrix(osg::Matrix::translate(-20000000, 0, 0));
    transform0->addChild(mapNode0);

    osg::MatrixTransform* transform1 = new osg::MatrixTransform();

    transform1->setMatrix(osg::Matrix::translate(20000000, 0, 0));
    transform1->addChild(mapNode1);

    osg::MatrixTransform* transform2 = new osg::MatrixTransform();

    transform2->setMatrix(osg::Matrix::translate(0, 0, 0));
    transform2->addChild(node2);

    osg::Group* root = new osg::Group();

    root->addChild(transform0);
    root->addChild(transform1);
    root->addChild(transform2);

    osgEarth::EarthManipulator*    manipulator0 = new osgEarth::EarthManipulator();
    osgEarth::EarthManipulator*    manipulator1 = new osgEarth::EarthManipulator();
    osgGA::NodeTrackerManipulator* manipulator2 = new osgGA::NodeTrackerManipulator();

    manipulator0->setNode(mapNode0);
    manipulator1->setNode(mapNode1);
    manipulator2->setNode(transform2);

    osgViewer::Viewer viewer;

    viewer.setCameraManipulator(manipulator0);

    ManipulatorController* manipulatorController = new ManipulatorController();

    manipulatorController->setViewer(&viewer);
    manipulatorController->setManipulator0(manipulator0);
    manipulatorController->setManipulator1(manipulator1);
    manipulatorController->setManipulator2(manipulator2);

    viewer.addEventHandler(manipulatorController);

    osgEarth::EarthManipulator::Settings* settings0 = manipulator0->getSettings();

    settings0->setLockAzimuthWhilePanning(false);
    settings0->setScrollSensitivity(1.5);
    settings0->setThrowingEnabled(true);
    settings0->setThrowDecayRate(0.02);
    settings0->setMinMaxDistance(1'000'000.0, 100'000'000.0);
    settings0->setArcViewpointTransitions(true);
    settings0->setAutoViewpointDurationEnabled(true);
    settings0->setAutoViewpointDurationLimits(0.0, 0.5);

    root->addEventCallback(new osg::NodeCallback());

    viewer.setUpViewInWindow(0, 0, 1920, 1080, 0);

    viewer.setSceneData(root);

    return viewer.run();
}