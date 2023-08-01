#include "SelectionFrameSystem.hpp"

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/Viewer>

SelectionFrameSystem::SelectionFrameSystem()
{
    osg::Box* frame = new osg::Box(osg::Vec3(), 1.0f);

    osg::ShapeDrawable* frameDrawable = new osg::ShapeDrawable(frame);

    osg::Geode* frameGeode = new osg::Geode();

    frameGeode->addDrawable(frameDrawable);

    m_frameSwitch = new osg::Switch();

    m_frameTransform = new osg::MatrixTransform();
    
    m_frameSwitch->addChild(m_frameTransform.get());

    m_frameTransform->setNodeMask(0x1);
    m_frameTransform->addChild(frameGeode);

    osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

    osg::StateSet* stateSet = m_frameTransform->getOrCreateStateSet();

    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setAttributeAndModes(polygonMode);
}

osg::Node* SelectionFrameSystem::getSelectionFrame()
{
    return m_frameSwitch.get();
}

bool SelectionFrameSystem::handle(const osgGA::GUIEventAdapter& eventAdapter, osgGA::GUIActionAdapter& actionAdapter)
{
    const osgGA::GUIEventAdapter::EventType eventType   = eventAdapter.getEventType();
    const int32_t                           mouseButton = eventAdapter.getButton();

    const bool b_buttonReleased = (eventType == osgGA::GUIEventAdapter::RELEASE);
    const bool b_leftButton     = (mouseButton == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);

    if (b_buttonReleased && b_leftButton)
    {
        OSG_DEBUG << "Double clicked";

        m_frameSwitch->setAllChildrenOff();

        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&actionAdapter);

        if (viewer)
        {
            const float x = eventAdapter.getX();
            const float y = eventAdapter.getY();

            osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);

            osgUtil::IntersectionVisitor intersectionVisitor(intersector.get());

            intersectionVisitor.setTraversalMask(~0x1);

            viewer->getCamera()->accept(intersectionVisitor);

            if (intersector->containsIntersections())
            {
                osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();

                const osgUtil::LineSegmentIntersector::Intersection& intersection = *(intersections.begin());

                const osg::BoundingBox bound = intersection.drawable->getBoundingBox();

                const osg::Matrix localToWorldMatrix = osg::computeLocalToWorld(intersection.nodePath);

                const osg::Vec3 boundCenterWorld = bound.center() * localToWorldMatrix;

                const float x = bound.xMax() - bound.xMin();
                const float y = bound.yMax() - bound.yMin();
                const float z = bound.zMax() - bound.zMin();

                const osg::Matrix scale = osg::Matrix::scale(x, y, z);

                const osg::Matrix matrixWorld = scale * localToWorldMatrix;

                m_frameTransform->setMatrix(matrixWorld);

                m_frameSwitch->setAllChildrenOn();
            }
        }
    }

    return false;
}