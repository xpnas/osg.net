#pragma once

#include <osgDB/ReadFile> 
#include <osgViewer/Viewer> 
#include <osg/Group> 
#include <osgFX/Scribe>
#include <osgGA/GUIEventHandler> 
#include <osgUtil/LineSegmentIntersector>

class CPickHandler : public osgGA::GUIEventHandler
{
public:
	CPickHandler(osgViewer::Viewer* viewer, osg::Node * pickModelNode) :mViewer(viewer), mPickModelNode(pickModelNode){}
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
	void Pick(float x, float y);
private:
	osgViewer::Viewer* mViewer;

  osg::Node * mPickModelNode;
};