#include "pch.h"
#include "Actor.h"
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
QFAEngineClassOut(QActor, QFAClass::ObjectClasses::Actor);
QActor::QActor()
{

}

QActor::~QActor()
{
	RootComponent->Destroy();

	if (ActorWorld->IsValid())
		ActorWorld->RemoveActor(this);
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
		return false;

	bool componentValide = component->IsValid();
	if (!componentValide)
	{
		if (RootComponent->IsValid())
		{
			RootComponent->IRootComponent = false;
			RootComponent->ParentActor = nullptr;
		}
		
		RootComponent = nullptr;
		return false;
	}

	if (component->Inseparable)
		return false;


	QSceneComponent* oldRootComponent = RootComponent;
	if (RootComponent)
	{
		RootComponent->IRootComponent = false;
		RootComponent->ParentActor = nullptr;			
	}

	if (component->ParentActor->IsValid())
	{
		component->ParentActor->SetRootComponent(nullptr);
		
	}
	else if (component->ParentActorComponent->IsValid())
	{
		component->ParentActorComponent->ForgetComponent(component);		
	}	
	
	RootComponent = component;	
	RootComponent->IRootComponent = true;
	RootComponent->Inseparable = inseparable;
	RootComponent->ParentActor = this;
	RootComponent->SetWorldPosition(Position);
	RootComponent->SetRotation(Rotation);
	RootComponent->SetScale(Scale);
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