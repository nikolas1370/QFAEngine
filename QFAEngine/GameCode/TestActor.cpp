#include "TestActor.h"
#include <iostream>




ATestActor::ATestActor()
{
	CanTick = false;
	/*----- 1D -----*/

	QFAInputAxis1D aa = input.CreateAxis1D("QFA", [](float axis)
		{
			std::cout << "Axis1D " << " " << axis << "\n";
		});

	aa.AddKey(EKey::Q, -1, "left");
	aa.AddKey(EKey::E, 1, "right");
	aa.RemoveKey("right");
	/*----- 2D -----*/

	QFAInputAxis2D aa2d = input.CreateAxis2D("QFA2D", [](FVector2D axis)
		{
			std::cout << "Axis2D " << " " << axis << "\n";
		});

	aa2d.AddKey(EKey::UP, FVector2D(1, 0), "forward");
	aa2d.AddKey(EKey::DOWN, FVector2D(-1, 0), "backward");
	aa2d.AddKey(EKey::LEFT, FVector2D(0, -1), "left");
	aa2d.AddKey(EKey::RIGHT, FVector2D(0, 1), "right");
	aa2d.RemoveKey("right");
	/*----- 3D -----*/

	QFAInputAxis3D aa3d = input.CreateAxis3D("QFA3D", [](FVector axis)
		{
			std::cout << "Axis3D " << " " << axis << "\n";
		});

	aa3d.AddKey(EKey::W, FVector(1, 0, 0), "forward");
	aa3d.AddKey(EKey::S, FVector(-1, 0, 0), "backward");
	aa3d.AddKey(EKey::A, FVector(0, -1, 0), "left");
	aa3d.AddKey(EKey::D, FVector(0, 1, 0), "right");
	aa3d.AddKey(EKey::SPACE, FVector(0, 0, 1), "up");
	aa3d.AddKey(EKey::LEFT_CONTROL, FVector(0, 0, -1), "down");
	aa3d.RemoveKey("down");
	/*----- mose wheel -----*/

	input.SetWheelAxis([](float axis)
		{
			std::cout << "Wheel move " << " " << axis << "\n";
		});	
}

ATestActor::~ATestActor()
{
}



void ATestActor::Tick(float deltaTime)
{
	std::cout << "I from Tick and deltaTime = " << deltaTime << "\n";
}

