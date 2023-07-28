#include <osg/Transform>

namespace test
{

class AutoScaleTransform : public osg::Transform
{
    public:

        AutoScaleTransform();

        AutoScaleTransform(const AutoScaleTransform& copy, const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);

        AutoScaleTransform(const osg::Vec3& translation, const osg::Quat& rotation);

        META_Node(test, AutoScaleTransform);

        virtual void traverse(osg::NodeVisitor& nodeVisitor) override;

        virtual AutoScaleTransform* asAutoScaleMatrixTransform();
        virtual const AutoScaleTransform* asAutoScaleMatrixTransform() const;

        void setTranslation(const osg::Vec3& translation);

        osg::Vec3 getTranslation() const;

        void setRotation(const osg::Quat& rotation);

        osg::Quat getRotation() const;

        osg::Vec3 getScale() const;

        osg::Matrix computeMatrix() const;

        osg::Matrix computeInverseMatrix() const;

        virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nodeVisitor) const override;
        
        virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nodeVisitor) const override;

    protected:

        virtual ~AutoScaleTransform();

        osg::Vec3 m_translation;
        osg::Quat m_rotation;
        osg::Vec3 m_scale;

};

} // namespace test