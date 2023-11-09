#include "TestActor.h"
#include <iostream>

ATestActor::ATestActor()
{
	CanTick = false;

}

ATestActor::~ATestActor()
{
}

void ATestActor::Tick(float deltaTime)
{
	std::cout << "I from Tick and deltaTime = " << deltaTime << "\n";
}

