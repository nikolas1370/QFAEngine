#include "Actor.h"
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
QActor::QActor()
{

}

QActor::~QActor()
{
	RootComponent->Destroy();

	if (ActorWorld)
		ActorWorld->ForgetActor(this);
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
{	// � ���� �� ������� �� ��������� ������

	FVector fv = FVector::ForwardVector;
	//RotationRelativeGlobal = true

	// Rotation Relative to global coordinat  
	fv = fv.RotateAngleAxis(Rotation.Y * -1, FVector::RightVector);
	fv = fv.RotateAngleAxis(Rotation.Z, FVector::UpVector);

	//

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



QSceneComponent* QActor::SetRootComponent(QSceneComponent* component)
{	
	
	QSceneComponent* oldRootComponent = RootComponent;
	if (RootComponent)
	{
		RootComponent->IRootComponent = false;
		RootComponent->ParentActor = nullptr;	
		
	}

	if (!component)
	{
		
		RootComponent = nullptr;
		return oldRootComponent;
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
	RootComponent->ParentActor = this;
	RootComponent->SetWorldPosition(Position);
	RootComponent->SetRotation(Rotation);
	RootComponent->SetScale(Scale);
	return oldRootComponent;
}



