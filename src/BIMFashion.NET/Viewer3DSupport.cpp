// Windows_OSG.cpp : implementation of the Viewer3DSupport class
//
#include "stdafx.h"
#include "Viewer3DSupport.h"

#include <osg/Geode> 
#include <osg/Geometry> 
#include <osg/ShapeDrawable> 
#include <osg/Shape>
#include <osgFX/Scribe>
#include <osg/BlendColor>  
#include <osg/BlendFunc>
#include <osg/MatrixTransform>

#include "PickHandler.h"


Viewer3DSupport::Viewer3DSupport(HWND hWnd) :
   m_hWnd(hWnd)
{
}

Viewer3DSupport::~Viewer3DSupport()
{
    mViewer->setDone(true);
    Sleep(1000);
    mViewer->stopThreading();

    delete mViewer;
}

void Viewer3DSupport::LoadFile(std::string modelname)
{
    // Store the name of the model to load
    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
}

void Viewer3DSupport::InitManipulators(void)
{
    // Create a trackball manipulator
    trackball = new osgGA::TrackballManipulator();
    trackball->setAllowThrow(false);

    // Create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value


}

//向场景中添加光源
osg::ref_ptr<osg::Group> Viewer3DSupport::createLight(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);

	//计算包围盒
	osg::BoundingSphere bs;
	node->computeBound();
	bs = node->getBound();


	//创建一个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	light->setLightNum(0);
	//设置方向
	light->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	//设置位置
	light->setPosition(osg::Vec4(bs.center().x(), bs.center().y(), bs.center().z() + bs.radius(), 1.0f));
	//设置环境光的颜色
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	//设置恒衰减指数
	light->setConstantAttenuation(1.0f);
	//设置线形衰减指数
	light->setLinearAttenuation(0.0f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.0f);

	//创建光源
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	lightSource->setLight(light.get());


	lightRoot->addChild(lightSource.get());

	return lightRoot.get();
}

void Viewer3DSupport::InitSceneGraph(void)
{
	// Init the main Root Node/Group
	mRoot = new osg::Group;

	// Load the Model from the model name
	mStaticModel = osgDB::readNodeFile(m_ModelName);
	if (!mStaticModel)
		return;

	//向场景中添加光源
  //mRoot->addChild(createLight(mModel.get()));

  // Optimize the model
	osgUtil::Optimizer optimizer;
	optimizer.optimize(mStaticModel.get());
	optimizer.reset();

	//SetTransparency(mStaticModel, 0.2);

	// Add the model to the scene
	mRoot->addChild(mStaticModel.get());

	//Add the mark
  //osg::ref_ptr<osgFX::Scribe> sc = new osgFX::Scribe();
  //sc->addChild(createShape().get());

	mDynamicModel = new osg::MatrixTransform();
	mDynamicModel->asTransform()->addChild(createMark().get());

	//mRoot->addChild(mDynamicModel.get());

}

void Viewer3DSupport::SetTransparency(osg::ref_ptr<osg::Node> node, double cy)
{
	osg::StateSet* stateset = node->getOrCreateStateSet();
	//Alpha混合开启
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//取消深度测试
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	stateset->setRenderBinDetails(11, "RenderBin");

	//使用BlendFunc实现透明效果  
	osg::ref_ptr<osg::BlendColor> bc = new osg::BlendColor(osg::Vec4(1.0, 1.0, 1.0, 0.0));
	osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc();
	stateset->setAttributeAndModes(bf, osg::StateAttribute::ON);
	stateset->setAttributeAndModes(bc, osg::StateAttribute::ON);
	bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
	bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
	bc->setConstantColor(osg::Vec4(1, 1, 1, cy));
}

void Viewer3DSupport::InitCameraConfig(void)
{
	// Local Variable to hold window size data
	RECT rect;

	// Create the viewer for this window
	mViewer = new osgViewer::Viewer();

	// Add a Stats Handler to the viewer
	mViewer->addEventHandler(new osgViewer::StatsHandler);

	// Get the current window size
	::GetWindowRect(m_hWnd, &rect);

	// Init the GraphicsContext Traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

	// Init the Windata Variable that holds the handle for the Window to display OSG in.
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->width = rect.right - rect.left;
	traits->height = rect.bottom - rect.top;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	traits->inheritedWindowData = windata;

	// Create the Graphics Context
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	// Init Master Camera for this View
	osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();

	// Assign Graphics Context to the Camera
	camera->setGraphicsContext(gc);

	// Set the viewport for the Camera
	//camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));

	// Set projection matrix and camera attribtues
	camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
	camera->setProjectionMatrixAsPerspective(
		30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 1.0, 1000.0);

	// Add the Camera to the Viewer
	//mViewer->addSlave(camera.get());
	mViewer->setCamera(camera.get());

	// Add the Camera Manipulator to the Viewer
	mViewer->setCameraManipulator(keyswitchManipulator.get());

	// Set the Scene Data
	mViewer->setSceneData(mRoot.get());

	mViewer->addEventHandler(new CPickHandler(mViewer, mDynamicModel));

	// Realize the Viewer
	mViewer->realize();

	// Correct aspect ratio
	/*double fovy,aspectRatio,z1,z2;
	mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
	aspectRatio=double(traits->width)/double(traits->height);
	mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void Viewer3DSupport::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void Viewer3DSupport::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

void Viewer3DSupport::ReplaceSceneData(std::string filename)
{ 
	//移掉mRoot中所有的数据,这些数据将会被自动释放掉，不用手动搞它 
	mRoot ->removeChildren(0, mRoot ->getNumChildren()) ; 
	
	//加入要置换的模型 
	mStaticModel = osgDB::readNodeFile(filename); 
	
	//优化 
	osgUtil::Optimizer optimizer; 
	optimizer.optimize(mStaticModel.get());
	optimizer.reset(); 
	
	//添加孩子 
	mRoot->addChild(mStaticModel.get()); 
}

osg::ref_ptr<osg::Node> Viewer3DSupport::createMark()
{ //几何组结点 
	osg::ref_ptr<osg::Geode> geode = new osg::Geode; 
	float radius = 500.0f; 
	float height = 1000.0f; 
	osg::TessellationHints* hints = new osg::TessellationHints; 
	hints->setDetailRatio(0.5f); 

   osg::ref_ptr<osg::ShapeDrawable> sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), radius), hints);
   sphere->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0)); //Red
   geode->addDrawable(sphere);
	
 //   geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.2f,0.0f,0.0f),2*radius),hints)); 
	//geode->addDrawable(new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0.4f,0.0f,0.0f),radius,height),hints));
	//geode->addDrawable(new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.60f,0.0f,0.0f),radius,height),hints)); 
	//geode->addDrawable(new osg::ShapeDrawable(new osg::Capsule(osg::Vec3(0.80f,0.0f,0.0f),radius,height),hints)); 
	return geode.get(); 
}




