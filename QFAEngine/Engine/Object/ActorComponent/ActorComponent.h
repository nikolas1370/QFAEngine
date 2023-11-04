#pragma once
#include <Object/Object.h>

class QActor;
/*
	in future be some do
*/
class QActorComponent : public QObject
{
	friend QActor;

protected:
	bool InWorldComponent = false;

public:
	QActorComponent();
	~QActorComponent();



private:

};
// staticMeshActor
