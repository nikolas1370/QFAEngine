#pragma once
#include <Object/Actor/Actor.h>
#include <Input/Input.h>
struct GLFWwindow;

class ATestActor : public QActor
{
	void Tick(float deltaTime) override;

	QFAInput input;
	QFAInput input2;
	QFAInput* input3;

public:
	ATestActor();
	~ATestActor();

private:

};
