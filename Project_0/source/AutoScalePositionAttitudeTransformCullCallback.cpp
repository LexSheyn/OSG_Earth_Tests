#include "AutoScalePositionAttitudeTransformCullCallback.hpp"

#include <osg/PositionAttitudeTransform>
#include <osgUtil/CullVisitor>

#include <iostream>

namespace test
{

AutoScalePositionAttitudeTransformCullCallback::AutoScalePositionAttitudeTransformCullCallback()
: m_scale          (osg::Vec3(1.0f, 1.0f, 1.0f))
, m_minScaleFactor (1.0f)
, m_maxScaleFactor (1.0f)
{
}

void AutoScalePositionAttitudeTransformCullCallback::setScale(const osg::Vec3& scale)
{
    m_scale = scale;
}

osg::Vec3 AutoScalePositionAttitudeTransformCullCallback::getScale() const
{
    return m_scale;
}

void AutoScalePositionAttitudeTransformCullCallback::setMinScaleFactor(float minScaleFactor)
{
    m_minScaleFactor = minScaleFactor;
}

float AutoScalePositionAttitudeTransformCullCallback::getMinScaleFactor() const
{
    return m_minScaleFactor;
}

void AutoScalePositionAttitudeTransformCullCallback::setMaxScaleFactor(float maxScaleFactor)
{
    m_maxScaleFactor = maxScaleFactor;
}

float AutoScalePositionAttitudeTransformCullCallback::getMaxScaleFactor() const
{
    return m_maxScaleFactor;
}

void AutoScalePositionAttitudeTransformCullCallback::operator()(osg::Node* node, osg::NodeVisitor* nodeVisitor)
{
    osg::NodeCallback::operator()(node, nodeVisitor);

    std::cout << "AutoScalePositionAttitudeTransformCullCallback::operator()" << std::endl;
    
    osg::PositionAttitudeTransform* transform = (osg::PositionAttitudeTransform*)node;

    osgUtil::CullVisitor* cullVisitor = (osgUtil::CullVisitor*)nodeVisitor;

    osg::Camera* camera = cullVisitor->getCurrentCamera();

    //cullVisitor->getDistanceToViewPoint();

    // If this is an render to texture (RTT) camera, we need to use it's
    // parent to calculate the proper scale factor.

    osg::View*   view         = camera->getView();
    osg::Camera* parentCamera = view->getCamera();

    if (camera->isRenderToTextureCamera() && view && parentCamera && parentCamera != camera)
    {
        camera = parentCamera;
    }

    osg::Viewport* viewport = camera->getViewport();

    if (viewport)
    {
        // Reset the scale so we get a proper bound.

        transform->setScale(m_scale);

        const osg::BoundingSphere& boundingSphere = node->getBound();

        // Transform centroid to view space.

        osg::Vec3 centerView = boundingSphere.center() * camera->getViewMatrix();

        // Set x coordinate to the radius so we can use the resulting clip
        // distance to calculate meters per pixel.

        const float boundingSpherRadius = boundingSphere.radius();

        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: boundingSpherRadius: " << boundingSpherRadius << std::endl;

        centerView.x() = boundingSpherRadius;

        // Transform the clip space.

        const osg::Vec3 centerClip = centerView * camera->getProjectionMatrix();

        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: centerClip x: " << centerClip.x() << std::endl;
        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: centerClip y: " << centerClip.y() << std::endl;
        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: centerClip z: " << centerClip.z() << std::endl;

        // Calculate meters per pixel.

        const float metersPerPixel = (centerClip.x() * 0.5f) * viewport->width();

        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: metersPerPixel: " << metersPerPixel << std::endl;

        // And the resulting scale we need to auto-scale.

        float scaleFactor = boundingSpherRadius / metersPerPixel;

        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: scaleFactor: " << scaleFactor << std::endl;

        if (scaleFactor < m_minScaleFactor)
        {
            scaleFactor = m_minScaleFactor;
        }
        else if (scaleFactor > m_maxScaleFactor)
        {
            scaleFactor = m_maxScaleFactor;
        }

        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: scaleFactor clamped: " << scaleFactor << std::endl;

        transform->setScale(osg::componentMultiply(m_scale, osg::Vec3(scaleFactor, scaleFactor, scaleFactor)));

        //transform->setScale(osg::Vec3(50000.0f, 50000.0f, 50000.0f));

        osg::Vec3 scaleVector = osg::componentMultiply(m_scale, osg::Vec3(scaleFactor, scaleFactor, scaleFactor));

        std::cout << std::fixed << "AutoScalePositionAttitudeTransformCullCallback: scale vector [ " << scaleVector.x() << " / " << scaleVector.y() << " / " << scaleVector.z() << " ]" << std::endl;
    }

    //if (node->getCullingActive() == false)
    //{
    //    node->setCullingActive(true);
    //}

    //this->traverse(node, nodeVisitor);

    //osg::NodeCallback::operator()(node, nodeVisitor);
}

} // namespace test