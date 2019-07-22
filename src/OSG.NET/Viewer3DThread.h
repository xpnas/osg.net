#pragma once

#include "Viewer3DSupport.h"



class Viewer3DThread : public OpenThreads::Thread
{
public:
	Viewer3DThread(Viewer3DSupport* ptr);
	virtual ~Viewer3DThread();

	virtual void run();


protected:
	Viewer3DSupport* _ptr;
	bool _done;
};
