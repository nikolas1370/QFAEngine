#include "TestActor.h"
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
#include <iostream>

ATestActor::ATestActor()
{
	CanTick = true;
}

ATestActor::~ATestActor()
{
}

void ATestActor::Tick(float delta)
{
	float cameraSpeed = 10;

}

