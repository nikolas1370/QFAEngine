#pragma once
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Vector.h>
#include <Math/Vector2D.h>

class ACameraActor : public QActor
{
	
protected:
	QCameraComponent Camera = QCameraComponent(45.0f, 1000.0f);
public:
	ACameraActor();
	~ACameraActor();

	void ActivateCamera();

	
private:

};
