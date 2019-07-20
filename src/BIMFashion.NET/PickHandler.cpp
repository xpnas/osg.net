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
	//����һ���߶ν�����⺯��
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
		//�õ�ѡ�������
		node = (nodePath.size() >= 1) ? nodePath[nodePath.size() - 1] : 0;
		parent = (nodePath.size() >= 2) ? dynamic_cast<osg::Group*>(nodePath[nodePath.size() - 2]) : 0;
	}

	//��һ�ָ�����ʾ����ʾ�����Ѿ���ѡ��
	if (parent.get() && node.get())
	{
		osg::ref_ptr<osgFX::Scribe> parentAsScribe = dynamic_cast<osgFX::Scribe*>(parent.get());
		if (!parentAsScribe)
		{
			//�������ѡ�񵽣�������ʾ
			osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe();
			scribe->addChild(node.get());
			parent->replaceChild(node.get(), scribe.get());
		}
		else
		{
			//���û��û��ѡ�񵽣����Ƴ�������ʾ�Ķ���
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

