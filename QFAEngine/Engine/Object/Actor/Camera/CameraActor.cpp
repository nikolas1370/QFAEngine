#include "CameraActor.h"

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
	Camera.Activate(viewport);
}
