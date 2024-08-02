#include "pch.h"
#include "World.h"
#include <Object/Actor/Actor.h>
#include <Overlord/Overlord.h>
#include <Overlord/Time.h>
#include <EngineStuff/Window/EngineViewport.h>
#include <EngineStuff/Window/EngineWindow.h>
#include <Tools/String.h>
QFAArray<QWorld*> QWorld::Worlds;
QFAEngineClassOut(QWorld, QFAClass::ObjectClasses::World);
void QWorld::ProcessTicks()
{	
	//std::cout << "Start\n";
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
		for (size_t j = 0; j < QFAEngineWindow::Windows.size(); j++)
		{
			if (!QFAEngineWindow::Windows[j]->ProcessTickIfWindowsMinimized && QFAEngineWindow::Windows[j]->minimized)
				continue;

			for (size_t k = 0; k < QFAEngineWindow::Windows[j]->Viewports.Length(); k++)
			{
				if (QFAEngineWindow::Windows[j]->Viewports[k]->GetWorld() == Worlds[i])
				{
					for (int o = 0; o < Worlds[i]->Actors.Length(); o++)
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
#if QFA_EDITOR_ONLY

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


#endif

void QWorld::DestroyWorld(bool deleteAllActor)
{
	if (deleteAllActor)
		for (size_t i = 0; i < Actors.Length(); i++)
			Actors[i]->Destroy();

	Destroy();
}