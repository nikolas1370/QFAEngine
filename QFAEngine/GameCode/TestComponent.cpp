#include "TestComponent.h"
#include <iostream>

void QTestComponent::TickComponent(float deltaTime)
{
	std::cout << "I run from TickComponent and deltatime = " << deltaTime << "\n";
}

QTestComponent::QTestComponent()
{
	CanTick = true;
}

QTestComponent::~QTestComponent()
{
}
