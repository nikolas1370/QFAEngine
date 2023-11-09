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
	if (!actor->IsValid())
		return;

	actor->ActorWorld = nullptr;
	Actors.Remove(actor);
}

QWorld::QWorld()
{
}

QWorld::~QWorld()
{
	for (int i = 0; i < Actors.Length(); i++)
		Actors[i]->Destroy();
}

void QWorld::AddActor(QActor* actor)
{
	if (!actor)
		return;

	if (actor->ActorWorld->IsValid())		
		actor->ActorWorld->ForgetActor(actor);

	actor->ActorWorld = this;


	Actors.Add(actor);
}
