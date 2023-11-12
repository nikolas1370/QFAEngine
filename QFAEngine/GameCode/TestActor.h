#pragma once
#include <Object/Actor/Actor.h>
#include <Input/Input.h>
struct GLFWwindow;

class ATestActor : public QActor
{
	void Tick(float deltaTime) override;

	
	

public:

	FVector inputAxis;
	FVector2D mouseAxis;
	bool mosePress;

	bool activeControl = false;

	ATestActor();
	~ATestActor();

private:

};
