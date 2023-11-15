#include "CameraEditor.h"

ACameraEditor::ACameraEditor()
{
	QFAInputAxis3D ax = Input.CreateAxis3D("move", [this](FVector axis)
		{
			this->inputAxis = axis;
		});

	Input.SetMouseMoveAxis([this](FVector2D axis)
		{
			this->mouseAxis = axis;
		});

	Input.AddKeyPress(EKey::MOUSE_LEFT, "mouse", [this](EKey::Key)
		{
			this->mosePress = true;
		});

	Input.AddKeyRelease(EKey::MOUSE_LEFT, "mouse", [this](EKey::Key)
		{
			this->mosePress = false;
		});

	ax.AddKey(EKey::W, FVector(1, 0, 0), "forward");
	ax.AddKey(EKey::S, FVector(-1, 0, 0), "backward");
	ax.AddKey(EKey::A, FVector(0, -1, 0), "left");
	ax.AddKey(EKey::D, FVector(0, 1, 0), "right");
	ax.AddKey(EKey::SPACE, FVector(0, 0, 1), "up");
	ax.AddKey(EKey::LEFT_CONTROL, FVector(0, 0, -1), "down");
	Camera.Activate();
}

ACameraEditor::~ACameraEditor()
{
}


void ACameraEditor::Tick(float delta)
{
	if (!Camera.GetStatus())
		return;

	if (mosePress)
	{
		FVector rot = GetActorRotation();
		rot.Y += mouseAxis.Y * 0.1f * -1;
		rot.Z += mouseAxis.X * 0.1f;

		if (rot.Y > 90)
			rot.Y = 90;
		else if (rot.Y < -90)
			rot.Y = -90;

		SetActorRotation(rot);
		mouseAxis = FVector2D(0);
	}

	SetActorPosition((GetActorForwardVector() * inputAxis.X + GetActorRightVector() * inputAxis.Y + GetActorUpVector() * inputAxis.Z).Normalize() * delta * Speed + GetActorPosition());
}
