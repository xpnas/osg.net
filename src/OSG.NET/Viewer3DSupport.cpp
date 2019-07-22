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


Viewer3DSupport::Viewer3DSupport(HWND hWnd) :
   m_hWnd(hWnd)
{
	InitManipulators();
	InitCameraConfig();
	InitSceneGraph();
}

Viewer3DSupport::~Viewer3DSupport()
{
	m_viewer->setDone(true);
	Sleep(1000);
	m_viewer->stopThreading();

	delete m_viewer;
}

void Viewer3DSupport::Load(std::string modelname)
{
	// Store the name of the model to load
	m_ModelName = modelname;

	// Init different parts of OSG

	m_fileRoot->removeChild(0, m_fileRoot->getNumChildren());
	m_lightRoot->removeChild(0, m_lightRoot->getNumChildren());

	// Load the Model from the model name
	osg::ref_ptr<osg::Node> fileNode = osgDB::readNodeFile(m_ModelName);
	if (!fileNode)
		return;

	// Optimize the model
	osgUtil::Optimizer optimizer;
	optimizer.optimize(fileNode.get());
	optimizer.reset();

	//向场景中添加光源
	osg::ref_ptr<osg::Node> lightNode = createLight(fileNode.get());

	m_fileRoot->addChild(fileNode.get());
	m_lightRoot->addChild(lightNode.get());

	// Set the Scene Data
	m_viewer->setSceneData(m_root.get());

}

void Viewer3DSupport::InitManipulators(void)
{
    // Create a trackball manipulator
    m_trackballManipulator = new osgGA::TrackballManipulator();
    m_trackballManipulator->setAllowThrow(false);

    // Create a Manipulator Switcher
    m_keySwitchMatrixManipulator = new osgGA::KeySwitchMatrixManipulator;
    // Add our trackball manipulator to the switcher
    m_keySwitchMatrixManipulator->addMatrixManipulator( '1', "Trackball", m_trackballManipulator.get());
    // Init the switcher to the first manipulator (in this case the only manipulator)
    m_keySwitchMatrixManipulator->selectMatrixManipulator(0);  // Zero based index Value

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
	light->setPosition(osg::Vec4(bs.center().x()+ bs.radius(), bs.center().y()+ bs.radius(), bs.center().z() + bs.radius(), 1.0f));
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
	m_root = new osg::Group;
	m_fileRoot = new osg::Group;
	m_lightRoot = new osg::Group;

	m_root->addChild(m_fileRoot.get());
	m_root->addChild(m_lightRoot.get());
}

void Viewer3DSupport::InitCameraConfig(void)
{
	// Local Variable to hold window size data
	RECT rect;

	// Create the viewer for this window
	m_viewer = new osgViewer::Viewer();

	// Add a Stats Handler to the viewer
	m_viewer->addEventHandler(new osgViewer::StatsHandler);

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
	traits->samples = 4;

	// Create the Graphics Context
	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

	// Init Master Camera for this View
	osg::ref_ptr<osg::Camera> camera = m_viewer->getCamera();

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
	m_viewer->setCamera(camera.get());

	// Add the Camera Manipulator to the Viewer
	m_viewer->setCameraManipulator(m_keySwitchMatrixManipulator.get());


	// Realize the Viewer
	m_viewer->realize();

}

void Viewer3DSupport::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void Viewer3DSupport::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}





