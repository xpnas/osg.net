#include "Viewer3DThread.h"




Viewer3DThread::Viewer3DThread(Viewer3DSupport* ptr)
	: OpenThreads::Thread(), _ptr(ptr), _done(false)
{

}

Viewer3DThread::~Viewer3DThread()
{
	_done = true;
	if (isRunning())
	{
		cancel();
		join();
	}
}

void Viewer3DThread::run()
{
	if (!_ptr)
	{
		_done = true;
		return;
	}

	osgViewer::Viewer* viewer = _ptr->getViewer();
	do
	{
		_ptr->PreFrameUpdate();
		viewer->frame();
		_ptr->PostFrameUpdate();
	} while (!testCancel() && !viewer->done() && !_done);
}