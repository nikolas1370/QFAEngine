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

	input3 = new QFAInput;

	input.AddKeyPress(EKey::A, [](EKey::Key key)
	{
			std::cout << "A \n";
	});

	input.AddKeyRelease(EKey::T, [](EKey::Key key)
		{
			std::cout << "T \n";
		});

	input.AddKeyPress(EKey::MOUSE_4, [](EKey::Key key)
		{
			std::cout << "mouse 4\n";
		});

	input2.AddKeyPress(EKey::B, [](EKey::Key key)
		{
			std::cout << "B \n";
		});

	input2.AddKeyRelease(EKey::R, [](EKey::Key key)
		{
			std::cout << "R \n";
		});

	input2.AddKeyRelease(EKey::MOUSE_LEFT, [](EKey::Key key)
		{
			std::cout << "Left mouse \n";
		});


	input3->AddKeyPress(EKey::Z, [](EKey::Key key)
		{
			std::cout << "Z \n";
		});

	//input.RemoveKeyPress(EKey::B);
	input.RemoveKeyPress(EKey::A);
	input2.RemoveKeyRelease(EKey::R);
	delete input3;
}

ATestActor::~ATestActor()
{
}



void ATestActor::Tick(float deltaTime)
{
	std::cout << "I from Tick and deltaTime = " << deltaTime << "\n";
}

