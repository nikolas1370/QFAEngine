#pragma once
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>

class QTestComponent : public QSceneComponent
{
	void TickComponent(float deltaTime) override;

public:
	QTestComponent();
	~QTestComponent();

private:

};

