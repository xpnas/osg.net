#include "stdafx.h"
#include "Viewer3D.h"
#include "Viewer3DSupport.h"
#include "Viewer3DThread.h"

namespace OSG
{
	namespace NET
	{
		Viewer3D::Viewer3D(IntPtr _hwnd)
		{
			//通过传入的句柄初始化osg对象
			m_support = new Viewer3DSupport((HWND)_hwnd.ToInt64());
			Start();
		}

		Viewer3D::~Viewer3D()
		{
			if (m_thread != nullptr)
			{
				delete m_thread;
				m_thread = nullptr;
			}
			if (m_support != nullptr)
			{
				delete m_support;
				m_support = nullptr;
			}
		}

		void Viewer3D::Start() {
			Stop();
			m_thread = new Viewer3DThread(m_support);
			m_thread->start();
		}

		void Viewer3D::Stop() {
			if (m_thread != nullptr)
			{
				m_thread->cancel();
				delete m_thread;
				m_thread = nullptr;
			}
		}

		void Viewer3D::Load(String ^ filePath)
		{
			std::string nativeStrId = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filePath);
			m_support->Load(nativeStrId);
			//创建渲染线程并开始渲染
			if (m_thread != nullptr)
			{
				delete m_thread;
				m_thread = nullptr;
			}

			m_thread = new Viewer3DThread(m_support);
			m_thread->start();
		}
	}
}
