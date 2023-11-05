#pragma once
#include <Object/Actor/Actor.h>

class ATestActor : public QActor
{
	void Tick(float deltaTime) override;

public:
	ATestActor();
	~ATestActor();

private:

};
