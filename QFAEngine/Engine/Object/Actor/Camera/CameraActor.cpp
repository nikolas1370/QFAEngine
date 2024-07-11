#include "pch.h"
#include "CameraActor.h"
#include <EngineStuff/Window/EngineViewport.h>
ACameraActor::ACameraActor()
{
	CanTick = true;
	SetRootComponent(&Camera, true);
}

ACameraActor::~ACameraActor()
{

}

void ACameraActor::ActivateCamera(QFAEngineViewport* viewport)
{

	Camera.Activate();	
	if (viewport)
		viewport->ChangeCamera(&Camera);	
}
