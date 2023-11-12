#pragma once
#include <Object/Actor/Actor.h>
#include <Input/Input.h>
struct GLFWwindow;

class ATestActor : public QActor
{
	void Tick(float deltaTime) override;

	
	

public:
	ATestActor();
	~ATestActor();

private:

};
