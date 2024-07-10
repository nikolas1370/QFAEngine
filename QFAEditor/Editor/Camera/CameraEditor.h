#pragma once
#include <Object/Actor/Camera/CameraActor.h>
#include <Input/Input.h>

class QFAEngineWindow;
class ACameraEditor : public ACameraActor
{

	FVector InputAxis;
	FVector2D MouseAxis;
	bool MosePress;
	QFAInput Input;
	float Speed = 1000;
protected:
	void Tick(float delta) override;
public:
	ACameraEditor();
	~ACameraEditor();

	void SetWindowForInput(QFAEngineWindow* window);

private:

};
