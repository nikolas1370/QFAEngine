#include "TestActor.h"
#include <iostream>




ATestActor::ATestActor()
{
	CanTick = false;
	

	input.SetPressedAnyKey([](EKey::Key)
		{
			std::cout << "SetPressedAnyKey\n";
		});

	input.ShutOffPressedAnyKey();


	input.AddKeyHold(EKey::A, [](EKey::Key key)
		{
			std::cout << "A\n";
		}, "QFA", 0.2);

	input.RemoveKeyHoldStrId("QFA");
}

ATestActor::~ATestActor()
{
}



void ATestActor::Tick(float deltaTime)
{
	std::cout << "I from Tick and deltaTime = " << deltaTime << "\n";
}

