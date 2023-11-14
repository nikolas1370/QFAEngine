#include "World.h"
#include <Object/Actor/Actor.h>
#include <Overlord/Overlord.h>
#include <Render/Time.h>

QFAArray<QWorld*> QWorld::Worlds;

void QWorld::ForgetActor(QActor* actor)
{
	if (!actor->IsValid())
		return;

	actor->ActorWorld = nullptr;
	Actors.Remove(actor);
}

void QWorld::ProcessTicks()
{	
	float delta = (float)QTime::GetDeltaTime();
	for (size_t i = 0; i < Worlds.Length(); i++)
	{
		if (!Worlds[i]->IsValid())
			continue;

		for (int j = 0; j < Worlds[i]->Actors.Length(); j++)
		{
			if (Worlds[i]->Actors[j]->IsValid() && Worlds[i]->Actors[j]->CanTick)
			{
				Worlds[i]->Actors[j]->Tick(delta);
				ProcessSceneComponentTick(Worlds[i]->Actors[j]->RootComponent);
			}
		}
	}
}

void QWorld::ProcessSceneComponentTick(QSceneComponent* component)
{
	if (!component->IsValid())
		return;

	float delta = (float)QTime::GetDeltaTime();
	if (component->CanTick)
		component->TickComponent(delta);

	for (int i = 0; i < component->ListComponents.Length(); i++)
		if (component->ListComponents[i]->IsValid())
			ProcessSceneComponentTick(component->ListComponents[i]);
}

QWorld::QWorld()
{
	Worlds.Add(this);
}

QWorld::~QWorld()
{
	for (int i = 0; i < Actors.Length(); i++)
		Actors[i]->Destroy();

	Worlds.Remove(this);
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
