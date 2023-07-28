#include "AutoScaleTransform.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace test
{

AutoScaleTransform::AutoScaleTransform()
: m_translation (osg::Vec3())
, m_rotation    (osg::Quat())
, m_scale       (osg::Vec3(1.0f, 1.0f, 1.0f))
{
}

AutoScaleTransform::AutoScaleTransform(const AutoScaleTransform& copy, const osg::CopyOp& copyOp)
: osg::Transform (copy, copyOp)
, m_translation  (copy.m_translation)
, m_rotation     (copy.m_rotation)
, m_scale        (copy.m_scale)
{
}

AutoScaleTransform::AutoScaleTransform(const osg::Vec3& translation, const osg::Quat& rotation)
{
    _referenceFrame = osg::Transform::RELATIVE_RF;

    m_translation = translation;
    m_rotation    = rotation;
    m_scale       = osg::Vec3(1.0f, 1.0f, 1.0f);
}

void AutoScaleTransform::traverse(osg::NodeVisitor& nodeVisitor)
{
    osg::Transform::traverse(nodeVisitor);

    const osg::BoundingSphere& boundingSphere = this->getBound();
    const osg::Vec3&           center         = boundingSphere.center();

    const float distanceToViewPoint = nodeVisitor.getDistanceToViewPoint(center, true);

    std::cout << std::fixed << "Distance to view point: " << distanceToViewPoint / 1'000.0f << std::endl;

    /*const float scaleValue = log2f(distanceToViewPoint);

    m_scale.x() = 1.0f + scaleValue;
    m_scale.y() = 1.0f + scaleValue;
    m_scale.z() = 1.0f + scaleValue;*/

    //const float scaleValue = distanceToViewPoint / 1'000.0f;
    const float scaleValue = 1.000001f;

    m_scale = osg::Vec3(scaleValue, scaleValue, scaleValue);
}

AutoScaleTransform* AutoScaleTransform::asAutoScaleMatrixTransform()
{
    return this;
}

const AutoScaleTransform* AutoScaleTransform::asAutoScaleMatrixTransform() const
{
    return this;
}

void AutoScaleTransform::setTranslation(const osg::Vec3& translation)
{
    m_translation = translation;
}

osg::Vec3 AutoScaleTransform::getTranslation() const
{
    return m_translation;
}

void AutoScaleTransform::setRotation(const osg::Quat& rotation)
{
    m_rotation = rotation;
}

osg::Quat AutoScaleTransform::getRotation() const
{
    return m_rotation;
}

osg::Vec3 AutoScaleTransform::getScale() const
{
    return m_scale;
}

osg::Matrix AutoScaleTransform::computeMatrix() const
{
    const osg::Matrix translation = osg::Matrix::translate(m_translation);
    const osg::Matrix rotation    = osg::Matrix::rotate(m_rotation);
    const osg::Matrix scale       = osg::Matrix::scale(m_scale);

    const osg::Matrix transform = scale * rotation * translation;

    return transform;
}

osg::Matrix AutoScaleTransform::computeInverseMatrix() const
{
    const osg::Matrix transform = this->computeMatrix();
    const osg::Matrix inverseTransform = osg::Matrix::inverse(transform);

    return inverseTransform;
}

bool AutoScaleTransform::computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nodeVisitor) const
{
    //return osg::Transform::computeLocalToWorldMatrix(matrix, nodeVisitor);

    const osg::Matrix transform = this->computeMatrix();

    std::cout << "AutoScaleTransform::computeLocalToWorldMatrix: " << transform.isNaN() << std::endl;

    if (_referenceFrame == osg::Transform::RELATIVE_RF)
    {
        matrix.preMult(transform);
    }
    else if (_referenceFrame == osg::Transform::ABSOLUTE_RF)
    {
        matrix = transform;
    }

    return true;
}

bool AutoScaleTransform::computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nodeVisitor) const
{
    //return osg::Transform::computeWorldToLocalMatrix(matrix, nodeVisitor);

    const osg::Matrix inverseTransform = this->computeInverseMatrix();

    std::cout << "AutoScaleTransform::computeWorldToLocalMatrix: " << inverseTransform.isNaN() << std::endl;

    if (_referenceFrame == osg::Transform::RELATIVE_RF)
    {
        matrix.preMult(inverseTransform);
    }
    else if (_referenceFrame == osg::Transform::ABSOLUTE_RF)
    {
        matrix = inverseTransform;
    }

    return true;
}

AutoScaleTransform::~AutoScaleTransform()
{
}

} // namespace test