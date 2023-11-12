#pragma once
#include <Object/Actor/Camera/CameraActor.h>
#include <Input/Input.h>

// imigrated to editor
class ACameraEditor : public ACameraActor
{

	FVector inputAxis;
	FVector2D mouseAxis;
	bool mosePress;
	QFAInput Input;
	float Speed = 10;
protected:
	void Tick(float delta) override;
public:
	ACameraEditor();
	~ACameraEditor();

private:

};
