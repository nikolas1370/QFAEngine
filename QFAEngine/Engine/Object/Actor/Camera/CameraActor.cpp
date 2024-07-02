#include "pch.h"
#include "CameraActor.h"
#include <Render/Window/Viewport.h>
ACameraActor::ACameraActor()
{
	CanTick = true;
	SetRootComponent(&Camera, true);
}

ACameraActor::~ACameraActor()
{

}

void ACameraActor::ActivateCamera(QFAViewport* viewport)
{

	Camera.Activate();	
	if (viewport)
		viewport->ChangeCamera(&Camera);	
}
