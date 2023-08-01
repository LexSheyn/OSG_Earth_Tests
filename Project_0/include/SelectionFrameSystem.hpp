#include <osg/Switch>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventHandler>

class SelectionFrameSystem : public osgGA::GUIEventHandler
{
public:

    SelectionFrameSystem();

    osg::Node* getSelectionFrame();

    // osgGA::GUIEventHandler.

    virtual bool handle(const osgGA::GUIEventAdapter& eventAdapter, osgGA::GUIActionAdapter& actionAdapter) override;

protected:

    osg::ref_ptr<osg::Switch>          m_frameSwitch;
    osg::ref_ptr<osg::MatrixTransform> m_frameTransform;
};