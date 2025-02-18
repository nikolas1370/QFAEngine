#pragma once
#include <Object/Object.h>
#include <Core/EngineStuff/EngineClass.h>
class QWorld;
class QActor;
/*
	in future be some do
*/
class QFAEXPORT QActorComponent : public QObject
{
	QFAEngineClassIn(QActorComponent);
	friend QWorld;
	friend QActor;

protected:
	// remove and see what happens
	bool InWorldComponent = false;

	bool CanTick = false;
	virtual void TickComponent(float deltaTime) {};
public:
	QActorComponent();
	~QActorComponent();
};
