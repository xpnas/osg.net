#pragma once

#include "Viewer3DSupport.h"
#include "Viewer3DThread.h"

using namespace System;

namespace BF
{
	namespace NET
	{
		public ref class Viewer3D
		{

		public:
			delegate void Progress(Viewer3D^, bool);
			event System::Action<bool> ^ OnLoad;
			event System::Action<bool> ^ OnPostFrame;
			event System::Action<bool> ^ OnPreFrame;
			event Progress^ OnProgress;

			Viewer3D(IntPtr _hwnd);

			~Viewer3D();

			void Load(String ^ filePath);

			void Start();

			void Stop();

		private:

			//����ȫ��Ψһ����
			Viewer3DSupport * m_support = nullptr;
			//��Ⱦ�߳�ȫ�ֱ���
			Viewer3DThread* m_thread = nullptr;
		};
	}
}

