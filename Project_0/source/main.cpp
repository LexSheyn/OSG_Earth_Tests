#include "AutoScalePositionAttitudeTransformCullCallback.hpp"

#include <osg/PositionAttitudeTransform>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/ProxyNode>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgUtil/Optimizer>

#include <osgEarth/Notify>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ExampleResources>
#include <osgEarth/MapNode>
#include <osgEarth/Threading>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Metrics>

#include <osgEarth/GeoTransform>

#include <iostream>

osg::Node* createBoundingBox(float radius)
{
    osg::Box* box = new osg::Box(osg::Vec3(), 1.0f);

    osg::ShapeDrawable* boxDrawable = new osg::ShapeDrawable(box);

    osg::Geode* geode = new osg::Geode();

    geode->addDrawable(boxDrawable);

    osg::MatrixTransform* boxTransform = new osg::MatrixTransform();

    boxTransform->addChild(geode);

    osg::PolygonMode* boxPolygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

    osg::StateSet* boxStateSet = boxTransform->getOrCreateStateSet();

    boxStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    boxStateSet->setAttributeAndModes(boxPolygonMode);

    const osg::Matrix scale = osg::Matrix::scale(radius, radius, radius);

    const osg::Matrix boxMatrix = boxTransform->getMatrix();

    const osg::Matrix newBoxMatrix = scale * boxMatrix;

    boxTransform->setMatrix(newBoxMatrix);

    return boxTransform;
}

struct ModelCreateInfo_osg
{
    osg::Vec3      translation;
    osg::Quat      rotation;
    osg::Vec3      scale;
    std::string    filePath;
    bool           b_multithreaded;
};

osg::Node* createModel_osg(const ModelCreateInfo_osg& createInfo)
{
    osg::Node* model = nullptr;
    
    if (createInfo.b_multithreaded)
    {
        osg::ProxyNode* proxyNode = new osg::ProxyNode();

        proxyNode->setFileName(0, createInfo.filePath);

        model = proxyNode;
    }
    else
    {
        model = osgDB::readNodeFile(createInfo.filePath);
    }

    osg::MatrixTransform* modelTransform = new osg::MatrixTransform();

    const osg::Matrix modelTranslation = osg::Matrix::translate(createInfo.translation);
    const osg::Matrix modelRotation    = osg::Matrix::rotate(createInfo.rotation);
    const osg::Matrix modelScale       = osg::Matrix::scale(createInfo.scale);

    const osg::Matrix modelMatrix = modelScale * modelRotation * modelTranslation;

    modelTransform->setMatrix(modelMatrix);

    modelTransform->addChild(model);

    return modelTransform;
}

struct ModelCreateInfo_osgEarth
{
    osgEarth::GeoPoint    geoPoint;
    std::string           filePath;
    bool                  b_multithreaded;
};

osg::Node* createModel_osgEarth(const ModelCreateInfo_osgEarth& createInfo)
{
    osg::Node* model = nullptr;
    
    if (createInfo.b_multithreaded)
    {
        osg::ProxyNode* proxyNode = new osg::ProxyNode();

        proxyNode->setFileName(0, createInfo.filePath);

        model = proxyNode;
    }
    else
    {
        model = osgDB::readNodeFile(createInfo.filePath);
    }

    test::AutoScalePositionAttitudeTransformCullCallback* cullCallback = new test::AutoScalePositionAttitudeTransformCullCallback();

    cullCallback->setScale(osg::Vec3(1.0f, 1.0f, 1.0f));
    cullCallback->setMinScaleFactor(5000.0f);
    cullCallback->setMaxScaleFactor(100000.0f);

    osg::PositionAttitudeTransform* positionAttitudeTransform = new osg::PositionAttitudeTransform();

    positionAttitudeTransform->setCullCallback(cullCallback);

    positionAttitudeTransform->addChild(model);

    osgEarth::GeoTransform* geoTransform = new osgEarth::GeoTransform();

    geoTransform->setPosition(createInfo.geoPoint);

    geoTransform->addChild(positionAttitudeTransform);

    return geoTransform;
}

#define LC "[viewer] "

using namespace osgEarth;
using namespace osgEarth::Util;

int
usage(const char* name)
{
    OE_NOTICE
        << "\nUsage: " << name << " file.earth" << std::endl
        << MapNodeHelper().usage() << std::endl;

    return 0;
}


int
main(int argc, char** argv)
{
    osgEarth::initialize();

    osg::ArgumentParser arguments(&argc,argv);

    // help?
    if ( arguments.read("--help") )
        return usage(argv[0]);

    // create a viewer:
    osgViewer::Viewer viewer(arguments);

    viewer.setUpViewInWindow(0, 0, 1920, 1080, 0);

    // This is normally called by Viewer::run but we are running our frame loop manually so we need to call it here.
    viewer.setReleaseContextAtEndOfFrameHint(false);

    // Tell the database pager to not modify the unref settings
    viewer.getDatabasePager()->setUnrefImageDataAfterApplyPolicy( true, false );

    // thread-safe initialization of the OSG wrapper manager. Calling this here
    // prevents the "unsupported wrapper" messages from OSG
    osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");

    // install our default manipulator (do this before calling load)
    viewer.setCameraManipulator( new EarthManipulator(arguments) );

    // disable the small-feature culling
    viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    // load an earth file, and support all or our example command-line options
    auto node = MapNodeHelper().load(arguments, &viewer);

    osgEarth::MapNode* mapNode = osgEarth::MapNode::get(node);

    osg::BoundingSphere mapBoundingSphere = mapNode->getBound();

    const float mapRadius = mapBoundingSphere.radius();

    osg::Node* mapBoundingBox = createBoundingBox(mapRadius);

    mapNode->addChild(mapBoundingBox);

    ModelCreateInfo_osgEarth modelCreateInfo;

    modelCreateInfo.geoPoint        = osgEarth::GeoPoint(mapNode->getMapSRS(), 45, 135, 500'000, osgEarth::ALTMODE_RELATIVE);
    modelCreateInfo.filePath        = "/home/user/Models/Blender/boeng_777_centered_osgEarth.osg";
    modelCreateInfo.b_multithreaded = true;

    osg::Node* model = createModel_osgEarth(modelCreateInfo);

    mapNode->addChild(model);

    class Callback : public osg::NodeCallback
    {
    public:

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nodeVisitor) override
        {
            auto bound = node->getBound();

            auto center = bound.center();

            std::cout << std::fixed << "Callback::operator(): center x: " << center.x() << std::endl;
            std::cout << std::fixed << "Callback::operator(): center y: " << center.y() << std::endl;
            std::cout << std::fixed << "Callback::operator(): center z: " << center.z() << std::endl;

            auto distanceToViewPoint = nodeVisitor->getDistanceToViewPoint(center, true);

            std::cout << std::fixed << "Callback::operator(): distanceToViewPoint: " << distanceToViewPoint << std::endl;
        
            osg::NodeCallback::operator()(node, nodeVisitor);
        }
    };

    Callback* callback = new Callback();

    //positionAttitudeTransform->addCullCallback(callback);
    //positionAttitudeTransform->setCullingActive(false);

    //mapNode->addChild(model);

    if (node)
    {
        viewer.setSceneData( node );
        
        if (!MapNode::get(node))
        {
            // not an earth file? Just view as a normal OSG node or image
            viewer.setCameraManipulator(new osgGA::TrackballManipulator);
            osgUtil::Optimizer opt;
            opt.optimize(node, osgUtil::Optimizer::INDEX_MESH);
            ShaderGenerator gen;
            node->accept(gen);
        }

        return Metrics::run(viewer);
    }

    return usage(argv[0]);
}