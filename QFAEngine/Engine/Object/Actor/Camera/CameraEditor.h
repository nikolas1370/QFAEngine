#pragma once
#include <Object/Actor/Camera/CameraActor.h>
#include <Input/Input.h>

class QFAWindow;
class ACameraEditor : public ACameraActor
{

	FVector inputAxis;
	FVector2D mouseAxis;
	bool mosePress;
	QFAInput Input;
	float Speed = 1000;
protected:
	void Tick(float delta) override;
public:
	ACameraEditor();
	~ACameraEditor();

	void SetWindowForInput(QFAWindow* window);

private:

};
