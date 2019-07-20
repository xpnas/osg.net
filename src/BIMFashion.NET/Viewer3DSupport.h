#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <string>

class Viewer3DSupport
{
public:
    Viewer3DSupport(HWND hWnd);
    ~Viewer3DSupport();

    void LoadFile(std::string filename);
    void InitManipulators(void);
    void InitSceneGraph(void);

    void InitCameraConfig(void);
	  osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node);

	  void ReplaceSceneData(std::string filename);

    void SetupWindow(void);
    void SetupCamera(void);
    void PreFrameUpdate(void);
    void PostFrameUpdate(void);
    void Done(bool value) { mDone = value; }
    bool Done(void) { return mDone; }
    //static void Render(void* ptr);

    osgViewer::Viewer* getViewer() { return mViewer; }

private:
	osg::ref_ptr<osg::Node> createMark();

  void SetTransparency(osg::ref_ptr<osg::Node> node, double cy);

private:
    bool mDone;
    std::string m_ModelName;
    HWND m_hWnd;
    osgViewer::Viewer* mViewer;
    osg::ref_ptr<osg::Group> mRoot;
    osg::ref_ptr<osg::Node> mStaticModel;
    osg::ref_ptr<osg::Node> mDynamicModel;
    osg::ref_ptr<osgGA::TrackballManipulator> trackball;
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;
};