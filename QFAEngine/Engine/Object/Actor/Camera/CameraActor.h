#pragma once
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Vector.h>
#include <Math/Vector2D.h>

class ACameraActor : public QActor
{
	QCameraComponent Camera = QCameraComponent(45.0f, 1000.0f);
protected:

public:
	ACameraActor();
	~ACameraActor();

	void ActivateCamera();

	/* remove */
	FVector inputAxis;
	FVector2D mouseAxis;
	bool mosePress;


	/*--*/

	bool activeControl = true;

	void Tick(float delta) override;
private:

};
