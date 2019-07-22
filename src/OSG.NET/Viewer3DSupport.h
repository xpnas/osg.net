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

    void Load(std::string filename);

    void SetupWindow(void);
    void SetupCamera(void);
    void PreFrameUpdate(void);
    void PostFrameUpdate(void);
    void Done(bool value) { m_done = value; }
    bool Done(void) { return m_done; }

    osgViewer::Viewer* getViewer() { return m_viewer; }

private:
    bool m_done;
    std::string m_ModelName;
    HWND m_hWnd;
    osgViewer::Viewer* m_viewer;
    osg::ref_ptr<osg::Group> m_root;
	osg::ref_ptr<osg::Group> m_fileRoot;
	osg::ref_ptr<osg::Group> m_lightRoot;
    osg::ref_ptr<osgGA::TrackballManipulator> m_trackballManipulator;
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> m_keySwitchMatrixManipulator;

	void InitManipulators(void);
	void InitSceneGraph(void);
	void InitCameraConfig(void);
	osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node);
};