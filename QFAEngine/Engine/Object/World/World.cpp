#include "World.h"
#include <Object/Actor/Actor.h>
#include <Overlord/Overlord.h>

void QWorld::Activate()
{
	if (!IsValid())
		return;

	IsActive = true;
	QFAOverlord::SetWorld(this);
}

void QWorld::Deactivate()
{
	if (!IsValid())
		return;

	IsActive = false;
}

void QWorld::ForgetActor(QActor* actor)
{
	for (int i = 0; i < ActorCount; i++)
	{
		if (Actors[i] == actor)
		{
			ActorCount--;
			Actors[i] = Actors[ActorCount];
			break;
		}
	}
}

QWorld::QWorld()
{
	Actors = new QActor*[ActorListLenght];// ActorListLenght
}

QWorld::~QWorld()
{
	for (size_t i = 0; i < ActorCount; i++)
		Actors[i]->Destroy();

	delete[] Actors;
}

void QWorld::AddActor(QActor* actor)
{
	if (!actor)
		return;

	if (actor->ActorWorld->IsValid())		
		actor->ActorWorld->ForgetActor(actor);

	actor->ActorWorld = this;
	if (ActorCount >= ActorListLenght)
	{
		QActor** tem = Actors;
		ActorListLenght *= 2;
		Actors = new QActor*[ActorListLenght];

		for (int i = 0; i < ActorCount; i++)
			Actors[i] = tem[i];
		
		delete[] tem;
	}

	Actors[ActorCount] = actor;
	ActorCount++;
}
