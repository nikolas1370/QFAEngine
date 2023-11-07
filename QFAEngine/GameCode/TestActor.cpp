#include "TestActor.h"
#include <iostream>




ATestActor::ATestActor()
{
	CanTick = false;
	//std::cout << "ATestActor::ATestActor\n";// <<
	//auto a = std::bind(&ATestActor::Key_callback, this);
	/*QFAInput::keyFun = [](int lo, int lo2, int lo3, int lo4)
		{
			std::cout << "(key)\n";// <<
		};*/


	input.AddKeyPress(EKey::A, [](EKey::Key key)
	{
			std::cout << "A \n";
	});

	int aaa = input.AddKeyPress(EKey::T, [](EKey::Key key)
		{
			std::cout << "T \n";
		});

	input.AddKeyPress(EKey::Y, [](EKey::Key key)
		{
			std::cout << "Y \n";
		}, "test");

	input.RemoveKeyPress(EKey::A);
	input.RemoveKeyPressId(aaa);
	input.RemoveKeyPressStrId("test");


	int aaa2 = input.AddKeyRelease(EKey::U, [](EKey::Key key)
		{
			std::cout << "U \n";
		});

	input.AddKeyRelease(EKey::I, [](EKey::Key key)
		{
			std::cout << "I \n";
		}, "test");

	input.RemoveKeyPress(EKey::A);
	input.RemoveKeyReleaseId(aaa2);
	input.RemoveKeyReleaseStrId("test");
}

ATestActor::~ATestActor()
{
}



void ATestActor::Tick(float deltaTime)
{
	std::cout << "I from Tick and deltaTime = " << deltaTime << "\n";
}

