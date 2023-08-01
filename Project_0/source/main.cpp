#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>

#include <osgEarth/MapNode>
#include <osgEarth/EarthManipulator>
#include <osgEarth/GeoTransform>

#include <iostream>
#include <cstdint>

class ManipulatorController : public osgGA::GUIEventHandler
{
public:

    ManipulatorController()
    : m_currentIndex (0)
    {}

    void addTarget(osgEarth::MapNode* target)
    {
        const osg::observer_ptr<osgEarth::MapNode> targetPointer = target;

        m_targets.push_back(targetPointer);
    }

    void removeTarget(uint32_t index)
    {
        m_targets.erase(m_targets.begin() + index);
    }

    void setTarget(uint32_t index, osgEarth::MapNode* target)
    {
        m_targets[index] = target;
    }
    
    const osgEarth::MapNode* getTarget(uint32_t index) const
    {
        osg::ref_ptr<osgEarth::MapNode> target;

        if (m_targets[index].lock(target))
        {
            return target.get();
        }

        return nullptr;
    }

    bool followTarget(uint32_t index)
    {
        osg::ref_ptr<osgEarth::MapNode> target;

        if (m_targets[index].lock(target))
        {
            osg::ref_ptr<osgEarth::EarthManipulator> manipulator;

            if (m_manipulator.lock(manipulator))
            {
                manipulator->setNode(target.get());

                return true;
            }
        }

        return false;
    }

    void setManipulator(osgEarth::EarthManipulator* manipulator)
    {
        m_manipulator = manipulator;
    }

    // osgGA::GUIEventHandler.

    virtual bool handle(const osgGA::GUIEventAdapter& eventAdapter, osgGA::GUIActionAdapter& actionAdapter) override
    {
        const osgGA::GUIEventAdapter::EventType eventType = eventAdapter.getEventType();

        if (eventType == osgGA::GUIEventAdapter::KEYDOWN)
        {
            const int32_t key = eventAdapter.getKey();

            if (!m_targets.empty())
            {
                switch (key)
                {
                    case 'q':
                    case 'Q':
                    {
                        m_currentIndex--;

                        if (m_currentIndex < m_targets.size())
                        {
                            const bool b_followed = this->followTarget(m_currentIndex);
                        }
                        else
                        {
                            m_currentIndex++;
                        }

                        break;
                    }

                    case 'e':
                    case 'E':
                    {
                        m_currentIndex++;

                        if (m_currentIndex < m_targets.size())
                        {
                            const bool b_followed = this->followTarget(m_currentIndex);
                        }
                        else
                        {
                            m_currentIndex--;
                        }

                        break;
                    }

                    default: break;
                }
            }
        }
    }

protected:

    virtual ~ManipulatorController() override
    {}

    std::vector<osg::observer_ptr<osgEarth::MapNode>> m_targets;
    osg::observer_ptr<osgEarth::EarthManipulator>     m_manipulator;
    uint32_t                                          m_currentIndex;
};

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