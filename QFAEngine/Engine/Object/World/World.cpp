#include "pch.h"
#include "World.h"
#include <Object/Actor/Actor.h>
#include <Core/QFA.h>
#include <Core/Time.h>
#include <Core/EngineStuff/Window/EngineViewport.h>
#include <Core/EngineStuff/Window/EngineWindow.h>
#include <Tools/String.h>
#include <Object/Class.h>
QFAArray<QWorld*> QWorld::Worlds;
QFAEngineClassOut(QWorld);

QWorld::QWorld()
{
	Worlds.Add(this);
}

QWorld::~QWorld()
{
	StopAndClearAudio(); // need if not used QWorld::DestroyWorld
	Worlds.Remove(this);
}

void QWorld::ProcessTicks()
{	
	const float delta = (float)QTime::GetDeltaTime();
	for (size_t i = 0; i < Worlds.Length(); i++) 
	{
		if (Worlds[i]->GetEnable())
		{
			for (int j = 0; j < Worlds[i]->Actors.Length(); j++)
			{
				if (Worlds[i]->Actors[j]->IsValid() && Worlds[i]->Actors[j]->CanTick)
				{
					Worlds[i]->Actors[j]->Tick(delta);
					ProcessSceneComponentTick(Worlds[i]->Actors[j]->RootComponent);
				}
			}
			
			continue; 
		}

		if (Worlds[i]->EditorActor->IsValid() && Worlds[i]->EditorActor->CanTick)
		{
			Worlds[i]->EditorActor->Tick(delta);
			ProcessSceneComponentTick(Worlds[i]->EditorActor->RootComponent);
		}
	}
}

void QWorld::ProcessSceneComponentTick(QSceneComponent* component)
{
	if (!component->IsValid())
		return;

	const float delta = (float)QTime::GetDeltaTime();
	if (component->CanTick)
		component->TickComponent(delta);

	for (int i = 0; i < component->ListComponents.Length(); i++)
		if (component->ListComponents[i]->IsValid())
			ProcessSceneComponentTick(component->ListComponents[i]);
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
	actor->SetActorPosition(actor->Position);
	actor->SetActorRotation(actor->Rotation);
	actor->SetActorScale(actor->Scale);
	SearchAndAddAudioInList(actor->RootComponent);
	actor->WasAddToWorld();
}

void QWorld::RemoveActor(QActor* actor)
{
	if (!actor->IsValid())
		return;

	actor->ActorWorld = nullptr;
	Actors.RemoveAt(actor->WorldIndex);
	Actors[actor->WorldIndex]->WorldIndex = actor->WorldIndex;

	SearchAndDeleteAudioInList(actor->RootComponent);
	actor->WasRemoveFromWorld();
}

void QWorld::SearchAndAddAudioInList(QSceneComponent* scene)
{
	if (!scene->IsValid())
		return;

	for (size_t i = 0; i < scene->ListComponents.Length(); i++)
	{
		if (scene->ListComponents[i]->GetClass()->GetId() == QFAObjectClasses::QOCAudioSceneComponent)
		{
			QWorld::SearchAndAddAudioInList(scene->ListComponents[i]);
			AddAudio((QAudioSceneComponent*)scene->ListComponents[i]);
		}
	}
}

void QWorld::SearchAndDeleteAudioInList(QSceneComponent* scene)
{
	if (!scene->IsValid())
		return;

	for (size_t i = 0; i < scene->ListComponents.Length(); i++)
	{
		if (scene->ListComponents[i]->GetClass()->GetId() == QFAObjectClasses::QOCAudioSceneComponent)
		{
			SearchAndDeleteAudioInList(scene->ListComponents[i]);
			RemoveAudio((QAudioSceneComponent*)scene->ListComponents[i]);
		}
	}
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
	StopAndClearAudio();	
	if (deleteAllActor)
		for (int i = Actors.Length() - 1; i >= 0; i--)
			Actors[i]->Destroy();

	Destroy();
}

void QWorld::AddAudio(QAudioSceneComponent* audio)
{
	if (!audio)
		return;

	for (size_t i = 0; i < AudioComponents.size(); i++)
		if (AudioComponents[i].AudioComponent == audio)
			return;

	audio->UpdateEmitterListenerParameters();
	AudioComponents.push_back({ audio });
	if (audio->ShouldPlay)
		audio->Play();
}

void QWorld::RemoveAudio(QAudioSceneComponent* audio)
{
	if (!audio)
		return;

	for (size_t i = 0; i < AudioComponents.size(); i++)
	{
		if (AudioComponents[i].AudioComponent == audio)
		{
			AudioComponents[i].AudioComponent->StopEngine();
			AudioComponents.erase(AudioComponents.begin() + i);
			return;
		}
	}
}

void QWorld::NewAudioActive(QCameraComponent* oldCamera)
{
	if (!this)
		return;

	if (QCameraComponent::MainCamera)
		if (QWorld* camWorld = QCameraComponent::MainCamera->GetWorld())
			if (this == camWorld)
				return;

	for (size_t i = 0; i < AudioComponents.size(); i++)
		AudioComponents[i].AudioComponent->StopEngine();
}

void QWorld::StopAndClearAudio()
{
	for (size_t i = 0; i < AudioComponents.size(); i++)
		AudioComponents[i].AudioComponent->StopEngine();

	AudioComponents.clear();
}
