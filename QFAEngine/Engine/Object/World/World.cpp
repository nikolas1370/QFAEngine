#include "pch.h"
#include "World.h"
#include <Object/Actor/Actor.h>
#include <Overlord/Overlord.h>
#include <Render/Time.h>
#include <Render/Window/Viewport.h>
#include <Render/Window/Window.h>

QFAArray<QWorld*> QWorld::Worlds;

void QWorld::ProcessTicks()
{	
	float delta = (float)QTime::GetDeltaTime();
	for (size_t i = 0; i < Worlds.Length(); i++) // search active world		
	{// world active if at least one viewport connect
		if (!Worlds[i]->GetEnable())
		{
			if (Worlds[i]->EditorActor->IsValid() && Worlds[i]->EditorActor->CanTick)
			{
				Worlds[i]->EditorActor->Tick(delta);
				ProcessSceneComponentTick(Worlds[i]->EditorActor->RootComponent);
			}

			continue;
		}

		bool worldProcess = false; // viewport connect to world by camera
		for (size_t j = 0; j < QFAWindow::Windows.size(); j++)
		{
			if (!QFAWindow::Windows[j]->ProcessTickIfWindowsMinimized && QFAWindow::Windows[j]->minimized)
				continue;

			for (size_t k = 0; k < QFAWindow::Windows[j]->Viewports.Length(); k++)
			{
				if (QFAWindow::Windows[j]->Viewports[k]->GetWorld() == Worlds[i])
				{
					for (int o = 0; j < Worlds[i]->Actors.Length(); o++)
					{
						if (Worlds[i]->Actors[o]->IsValid() && Worlds[i]->Actors[o]->CanTick)
						{
							Worlds[i]->Actors[o]->Tick(delta);
							ProcessSceneComponentTick(Worlds[i]->Actors[o]->RootComponent);
							worldProcess = true;
							break;
						}
					}

					if (worldProcess)
						break;
				}
			}

			if (worldProcess)
				break;
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
	Worlds.Remove(this);
}

void QWorld::AddActor(QActor* actor)
{
	if (!actor->IsValid())
		return;

	if (actor->ActorWorld->IsValid())		
		actor->ActorWorld->RemoveActor(actor);

	actor->ActorWorld = this;
	actor->WorldIndex = Actors.Length();
	Actors.Add(actor);
}

void QWorld::RemoveActor(QActor* actor)
{
	if (!actor->IsValid())
		return;

	actor->ActorWorld = nullptr;
	Actors.RemoveAt(actor->WorldIndex);
	Actors[actor->WorldIndex]->WorldIndex = actor->WorldIndex;
}

void QWorld::SetEditorActor(QActor* actor)
{
	if (!actor->IsValid())
	{
		if (EditorActor->IsValid())
			EditorActor->ActorWorld = nullptr;

		EditorActor = nullptr;
		return;
	}

	if (EditorActor->IsValid())
		EditorActor->ActorWorld = nullptr;

	actor->ActorWorld = this;
	EditorActor = actor;
}