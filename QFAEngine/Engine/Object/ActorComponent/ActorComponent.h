#pragma once
#include <Object/Object.h>

class QFAOverlord;
class QActor;
/*
	in future be some do
*/
class QActorComponent : public QObject
{
	friend QFAOverlord;
	friend QActor;

protected:
	bool InWorldComponent = false;


	bool CanTick = false;
	virtual void TickComponent(float deltaTime) {};
public:
	QActorComponent();
	~QActorComponent();



private:

};
// staticMeshActor
