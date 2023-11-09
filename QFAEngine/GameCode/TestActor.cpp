#include "TestActor.h"
#include <iostream>

ATestActor::ATestActor()
{
	CanTick = false;
	input.SetMouseMoveAxis([](FVector2D axis)
		{
			std::cout << axis.X << " " << axis.Y << std::endl;
		});
}

ATestActor::~ATestActor()
{
}

void ATestActor::Tick(float deltaTime)
{
	std::cout << "I from Tick and deltaTime = " << deltaTime << "\n";
}

