#include <osg/NodeCallback>
#include <osg/Vec3>

namespace test
{

class AutoScalePositionAttitudeTransformCullCallback : public osg::NodeCallback
{
public:

    AutoScalePositionAttitudeTransformCullCallback();

    void setScale(const osg::Vec3& scale);

    osg::Vec3 getScale() const;

    void setMinScaleFactor(float minScaleFactor);

    float getMinScaleFactor() const;

    void setMaxScaleFactor(float maxScaleFactor);

    float getMaxScaleFactor() const;

public:

    // osg::NodeCallback.

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nodeVisitor) override;

protected:

    osg::Vec3 m_scale;
    float     m_minScaleFactor;
    float     m_maxScaleFactor;
};

} // namespace test