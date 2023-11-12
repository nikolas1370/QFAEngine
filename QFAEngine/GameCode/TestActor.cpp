#include "TestActor.h"
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
#include <iostream>

ATestActor::ATestActor()
{
	CanTick = true;

	SetActorRotation(FVector(67, 0, 0));

	QFAInput* in = new QFAInput;
	in->AddKeyPress(EKey::Q, "activeControl", [this](EKey::Key)
		{
			this->activeControl = !this->activeControl;
		});

	QFAInputAxis3D ax = in->CreateAxis3D("move", [this](FVector axis)
		{
			this->inputAxis = axis;
			//std::cout << "lox 1" << " "<< axis << "\n";
		});

	in->SetMouseMoveAxis([this](FVector2D axis)
		{
			this->mouseAxis = axis;
		});

	in->AddKeyPress(EKey::MOUSE_LEFT, "mouse", [this](EKey::Key)
		{
			this->mosePress = true;
		});

	in->AddKeyRelease(EKey::MOUSE_LEFT, "mouse", [this](EKey::Key)
		{
			this->mosePress = false;
		});

	ax.AddKey(EKey::W, FVector(1, 0, 0), "forward");
	ax.AddKey(EKey::S, FVector(-1, 0, 0), "backward");
	ax.AddKey(EKey::A, FVector(0, -1, 0), "left");
	ax.AddKey(EKey::D, FVector(0, 1, 0), "right");
	ax.AddKey(EKey::SPACE, FVector(0, 0, 1), "up");
	ax.AddKey(EKey::LEFT_CONTROL, FVector(0, 0, -1), "down");

}

ATestActor::~ATestActor()
{
}

void ATestActor::Tick(float delta)
{
	float cameraSpeed = 10;

	if (!activeControl)
		return;

	if (mosePress)
	{
		FVector rot = RootComponent->GetRotation();
		rot.Y += mouseAxis.Y * 0.1f * -1;
		rot.Z += mouseAxis.X * 0.1f;

		if (rot.Y > 90)
			rot.Y = 90;
		else if (rot.Y < -90)
			rot.Y = -90;

		RootComponent->SetRotation(rot);
		mouseAxis = FVector2D(0);
	}

	// aaa == Arrow
	QSceneComponent* aaa = RootComponent->ListComponents[0]->ListComponents[0];

	SetActorPosition((aaa->GetForwardVector() * inputAxis.X + aaa->GetRightVector() * inputAxis.Y + aaa->GetUpVector() * inputAxis.Z).Normalize() * delta * cameraSpeed + RootComponent->GetWorldPosition());
	//std::cout << GetActorPosition() << "\n";
}

