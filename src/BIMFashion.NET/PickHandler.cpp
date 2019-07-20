#include "stdafx.h"
#include "PickHandler.h"

bool CPickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH):
		if (ea.getButton() == 1)
		{
			Pick(ea.getX(), ea.getY());
		}
		return true;
	}
	return false;
}

void  CPickHandler::Pick(float x, float y)
{

	osg::ref_ptr<osg::Node> node = new osg::Node();
	osg::ref_ptr<osg::Group> parent = new osg::Group();
	//创建一个线段交集检测函数
	osgUtil::LineSegmentIntersector::Intersections intersections;

	bool bCompute = false;
	if (mPickModelNode != NULL)
	{
		osg::NodePath pickNodePath;
		pickNodePath.push_back(mPickModelNode);
		bCompute = mViewer->computeIntersections(x, y, pickNodePath, intersections);
	}
	else
	{
		bCompute = mViewer->computeIntersections(x, y, intersections);
	}

	if (bCompute)
	{
		osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
		osg::NodePath& nodePath = intersection.nodePath;
		//得到选择的物体
		node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
		parent = (nodePath.size() >= 2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
	}

	//用一种高亮显示来显示物体已经被选中
	if (parent.get() && node.get())
	{
		osg::ref_ptr<osgFX::Scribe> parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent.get());
		if (!parentAsScribe)
		{
			//如果对象选择到，高亮显示
			osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe();
			scribe->addChild(node.get());
			parent->replaceChild(node.get(), scribe.get());
		}
		else
		{
			//如果没有没有选择到，则移除高亮显示的对象
			osg::Node::ParentList parentList = parentAsScribe->getParents();
			for (osg::Node::ParentList::iterator itr = parentList.begin();
				itr != parentList.end();
				++itr)
			{
				(*itr)->replaceChild(parentAsScribe.get(), node.get());
			}
		}
	}
}

