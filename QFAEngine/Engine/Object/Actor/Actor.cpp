#include "pch.h"
#include "Actor.h"
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
QFAEngineClassOut(QActor);
QActor::QActor()
{

}

QActor::~QActor()
{
	if(RootComponent->IsValid())
	{		
		RootComponent->ParentActor = nullptr;
		RootComponent->Destroy();
		RootComponent = nullptr;
	}

	if (ActorWorld->IsValid())
		ActorWorld->RemoveActor(this);
}

void QActor::SeparateRootComponent()
{
	if (RootComponent)
	{
		RootComponent->Inseparable = false;
		SetRootComponent(nullptr);
	}	
}

void QActor::SetActorPosition(const FVector& position)
{
	if (RootComponent)
		RootComponent->SetWorldPosition(position);
	else
		Position = position;
}

void QActor::SetActorRotation(const FVector& rotation)
{	
	if (RootComponent->IsValid())
		RootComponent->SetRotation(rotation);
	else
		Rotation = rotation;
}

void QActor::SetActorScale(const FVector& scale)
{
	if (RootComponent->IsValid())
		RootComponent->SetScale(scale);
	else
		Scale = scale;
}


FVector QActor::GetActorForwardVector() const
{
	/* work for camera  */
	FVector fv = FVector::ForwardVector;
	
	fv = fv.RotateAngleAxis(Rotation.Y * -1, FVector::RightVector);
	fv = fv.RotateAngleAxis(Rotation.Z, FVector::UpVector);
	return fv;
}

FVector QActor::GetActorRightVector() const
{
	FVector fv = FVector::RightVector;
	fv = fv.RotateAngleAxis(Rotation.X * -1, FVector::ForwardVector);
	fv = fv.RotateAngleAxis(Rotation.Y * -1, FVector::RightVector);
	fv = fv.RotateAngleAxis(Rotation.Z, FVector::UpVector);
	return fv;
}

FVector QActor::GetActorUpVector() const
{
	FVector fv = FVector::UpVector;
	fv = fv.RotateAngleAxis(Rotation.X * -1, FVector::ForwardVector);
	fv = fv.RotateAngleAxis(Rotation.Y * -1, FVector::RightVector);

	fv = fv.RotateAngleAxis(Rotation.Z, FVector::UpVector);
	return fv;
}

bool QActor::SetRootComponent(QSceneComponent* component, bool inseparable)
{	
	if (RootComponent->IsValid() && RootComponent->Inseparable)
	{
		stopExecute("can't change Inseparable RootComponent");
		return false;
	}

	if (!component->IsValid())
	{
		if (RootComponent->IsValid())
		{
			RootComponent->IRootComponent = false;
			RootComponent->ParentActor = nullptr;
			RootComponent->ParentChanged();
		}
		
		RootComponent = nullptr;
		return false;
	}

	if (component->Inseparable)
	{
		stopExecute("Inseparable Component can't change parent");
		return false;
	}

	QSceneComponent* oldRootComponent = RootComponent;
	if (RootComponent)
	{
		RootComponent->IRootComponent = false;
		RootComponent->ParentActor = nullptr;			
		RootComponent->ParentChanged();
	}

	if (component->ParentActor->IsValid())
		component->ParentActor->SetRootComponent(nullptr);
	else if (component->ParentActorComponent->IsValid())
		component->ParentActorComponent->ForgetComponent(component);

	RootComponent = component;	
	RootComponent->IRootComponent = true;
	RootComponent->Inseparable = inseparable;
	RootComponent->ParentActor = this;
	RootComponent->SetWorldPosition(Position);
	RootComponent->SetRotation(Rotation);
	RootComponent->SetScale(Scale);
	RootComponent->ParentChanged();
	return true;
}

#if QFA_EDITOR_ONLY

void QActor::ReplaceMe(QObject* newActor)
{
	if(ActorWorld)
	{
		QActor* actor = (QActor*)newActor;
		ActorWorld->Actors[WorldIndex] = actor;
		actor->WorldIndex = WorldIndex;
		ActorWorld = nullptr;
		actor->Name = Name;

		actor->SetActorPosition(Position);
		actor->SetActorRotation(Rotation);
		actor->SetActorScale(Scale);
	}
}

#endif 

void QActor::WasAddToWorld()
{
	if(RootComponent)
		RootComponent->ParentChanged();
}

void QActor::WasRemoveFromWorld()
{
	if (RootComponent)
		RootComponent->ParentChanged();
}