#include "pch.h"
#include "SceneComponent.h"
#include <Object/Actor/Actor.h>
#include <Object/World/World.h>
QFAEngineClassOut(QSceneComponent);

QSceneComponent::QSceneComponent()
{
	InWorldComponent = true;
}

QSceneComponent::~QSceneComponent()
{
	for (unsigned int i = 0; i < (unsigned int)ListComponents.Length(); i++)
		ListComponents[i]->Destroy();

	if (ParentActor)
	{		
		if (Inseparable)
			stopExecute("can't destroy it if Parent is QActor and this component Inseparable");

		ParentActor->SetRootComponent(nullptr);
	}
	else if(ParentActorComponent)
		ParentActorComponent->ForgetComponent(this);

}

void QSceneComponent::ParentChanged()
{
	for (size_t i = 0; i < ListComponents.Length(); i++)
		ListComponents[i]->ParentChanged();
} 

void QSceneComponent::ChangeWorldPosition(const FVector position)
{
	if (IRootComponent)
	{
		ParentActor->Position = position;
		RelativePosition = 0;
		AccumulateScale = FVector(1);
		WorldPosition = position.ConvertToVulkanCoordinate();
		UpdatePosition();
		for (unsigned int i = 0; i < (unsigned int)ListComponents.Length(); i++)
			if (ListComponents[i]->IsValid())
				ListComponents[i]->UpdateWorldPositionScale(true);
	}
	else if (ParentActorComponent->IsValid())
	{
		WorldPosition = position.ConvertToVulkanCoordinate();
		RelativePosition = position - ParentActorComponent->GetWorldPosition();
		RelativePosition = RelativePosition.ReversScale(AccumulateScale);
		//invers RotationMatrix rotate finally position to pure relative position(pure = not scale and rotate)
		RelativePosition = FVector(glm::inverse(ParentActorComponent->RotationMatrix) * RelativePosition.ConvertToVulkanCoordinate().GetGLMVector()).ConvertFromVulkanCoordinate();
		UpdatePosition();
		for (int i = 0; i < ListComponents.Length(); i++)
			if (ListComponents[i]->IsValid())
				ListComponents[i]->UpdateWorldPositionScale(true);
	}

	LocalPosition = WorldPosition.ConvertFromVulkanCoordinate() - GetActor()->GetActorPosition();
}

void QSceneComponent::ChangeLocalPosition(const FVector position)
{
	LocalPosition = position;
	if (IRootComponent || !ParentActorComponent->IsValid())
		return;

	QActor* temA = GetActor();
	if (!temA->IsValid())
		return;

	AccumulateScale = ParentActorComponent->AccumulateScale * ParentActorComponent->Scale;
	WorldPosition = (position + temA->GetActorPosition()).ConvertToVulkanCoordinate();
	RelativePosition = (WorldPosition - ParentActorComponent->WorldPosition).ConvertFromVulkanCoordinate();
	RelativePosition = RelativePosition.ReversScale(ParentActorComponent->AccumulateScale * ParentActorComponent->Scale);
	//invers RotationMatrix rotate finally position to pure relative position(pure = not scale and rotate)
	RelativePosition = FVector(glm::inverse(ParentActorComponent->RotationMatrix) * RelativePosition.ConvertToVulkanCoordinate().GetGLMVector()).ConvertFromVulkanCoordinate();
	UpdatePosition();
	for (int i = 0; i < ListComponents.Length(); i++)
		if (ListComponents[i]->IsValid())
			ListComponents[i]->UpdateWorldPositionScale(true);
}

void QSceneComponent::ChangeRelativePosition(const FVector position)
{
	RelativePosition = position;
	if (IRootComponent || !ParentActorComponent->IsValid())
		return;
	else
		UpdateWorldPositionScale(true);

	LocalPosition = WorldPosition.ConvertFromVulkanCoordinate() - GetActor()->GetActorPosition();
}


FVector QSceneComponent::GetLocalPosition()
{	
	return LocalPosition;
}


void QSceneComponent::ChangedParentRotation()
{// rotate component center(relative position) around parent whit parent rotateMatrix
	glm::vec3 ass = ParentActorComponent->RotationMatrix * GetRelativeScalePosition().ConvertToVulkanCoordinate().GetGLMVector();
	WorldPosition = ParentActorComponent->WorldPosition + FVector(ass);

		/*
	If you do your multiplication of the matrix from right to left,
	it will rotate around the global axis.
	If you do it the other way,
	it will rotate around the local axis.
		global   z x y
		local coordinat  x y z
	*/
	RotationMatrix = ParentActorComponent->RotationMatrix;// set parent rotation

	// set component rotation
	RotationMatrix = Math::rotateMatrix3(RotationMatrix, glm::radians(Rotation.Z * -1), glm::vec3(0.0f, 1.0f, 0.0f));
	RotationMatrix = Math::rotateMatrix3(RotationMatrix, glm::radians(Rotation.Y), glm::vec3(1.f, 0.f, 0.f));
	RotationMatrix = Math::rotateMatrix3(RotationMatrix, glm::radians(Rotation.X), glm::vec3(0.0f, 0.0f, -1.0f));
	if (NeedUpdateMatrix)
		UpdateModelMatrix();

	for (int i = 0; i < ListComponents.Length(); i++)
		if(ListComponents[i]->IsValid())
		ListComponents[i]->ChangedParentRotation();
}


void QSceneComponent::ChangeRotation(const FVector rotation)
{
	Rotation = rotation;
	if (IRootComponent)
	{
		ParentActor->Rotation = rotation;
		RotationMatrix = Math::DefauldMatrix3;
		RotationMatrix = Math::rotateMatrix3(RotationMatrix, glm::radians(Rotation.Z * -1), glm::vec3(0.0f, 1.0f, 0.0f));
		RotationMatrix = Math::rotateMatrix3(RotationMatrix, glm::radians(Rotation.Y), glm::vec3(1.f, 0.f, 0.f));
		RotationMatrix = Math::rotateMatrix3(RotationMatrix, glm::radians(Rotation.X), glm::vec3(0.0f, 0.0f, -1.0f));
		if (NeedUpdateMatrix)
			UpdateModelMatrix();

		for (int i = 0; i < ListComponents.Length(); i++)
			if (ListComponents[i]->IsValid())
				ListComponents[i]->ChangedParentRotation();
	}
	else if (ParentActorComponent->IsValid())
		ChangedParentRotation();
}

void QSceneComponent::UpdateWorldPositionScale(bool onlyPosition)
{
	AccumulateScale = ParentActorComponent->AccumulateScale * ParentActorComponent->Scale;
	glm::vec3 ass = ParentActorComponent->RotationMatrix * GetRelativeScalePosition().ConvertToVulkanCoordinate().GetGLMVector();
	WorldPosition = ParentActorComponent->WorldPosition + FVector(ass);
	if (NeedUpdateMatrix && !onlyPosition)
		UpdateModelMatrix();
	else 
		UpdatePosition();

	for (int i = 0; i < ListComponents.Length(); i++)
		if (ListComponents[i]->IsValid())
			ListComponents[i]->UpdateWorldPositionScale(onlyPosition);
}

void QSceneComponent::ChangeScale(const FVector scale)
{
	Scale = scale;
	if (IRootComponent)
	{		
		AccumulateScale = FVector(1);
		ParentActor->Scale = scale;
		if (NeedUpdateMatrix)
			UpdateModelMatrix();

		for (int i = 0; i < ListComponents.Length(); i++)
			if (ListComponents[i]->IsValid())
				ListComponents[i]->UpdateWorldPositionScale(false);
	}
	else if (ParentActorComponent->IsValid())
		UpdateWorldPositionScale(false);
}

FVector QSceneComponent::GetScale() const
{
	return Scale;
}

FVector QSceneComponent::GetForwardVector() const
{
	if (IRootComponent)
		return ParentActor->GetActorForwardVector();

	//glm::vec3 fv = glm::vec3(ToOpenglCoordinate(FVector::ForwardVector).GetGLMVector());
	//return FromOpenglCoordinate(FVector(RotationMatrix * fv)); // FVector(RotationMatrix * fv).ConvertFromVulkanCoordinate()
	return FVector::ForwardVector;
}

FVector QSceneComponent::GetRightVector() const
{
	if (IRootComponent)
		return ParentActor->GetActorRightVector();

	/*
	glm::vec3 fv = glm::vec3(ToOpenglCoordinate(FVector::RightVector).GetGLMVector());
	return FromOpenglCoordinate(FVector(RotationMatrix * fv));
	*/
	return FVector::RightVector;
}

FVector QSceneComponent::GetUpVector() const
{
	if (IRootComponent)
		return ParentActor->GetActorUpVector();

	/*
	glm::vec3 fv = glm::vec3(ToOpenglCoordinate(FVector::UpVector).GetGLMVector());
	return FromOpenglCoordinate(FVector(RotationMatrix * fv));
	*/
	return FVector::UpVector;
}

bool QSceneComponent::ItsMyFather(QSceneComponent* component)
{
	QSceneComponent* parent = ParentActorComponent;
	while (true)
	{
		if (!parent)
			return false;
		else if (parent == component)
			return true;

		parent = parent->GetParentComponent();
	}
}

void QSceneComponent::AttachComponent(QSceneComponent* component, bool inseparable)
{
	if (!component->IsValid() || component->Inseparable)
		return;	

	if (ItsMyFather(component))
	{
		std::cout << "QSceneComponent::AttachComponent component my Father. Father cannot be my child\n";
		__debugbreak();
		return;
	}

	if (component->ParentActor->IsValid())
		component->ParentActor->SetRootComponent(nullptr);
	else if (component->ParentActorComponent->IsValid())
		component->ParentActorComponent->ForgetComponent(component);

	component->ParentActorComponent = this;
	component->Inseparable = inseparable;
	ListComponents.Add(component);	
	component->ChangeLocalPosition(component->GetLocalPosition());
	component->ChangeRotation(component->GetRotation()); 
	component->ChangeScale(component->GetScale());

	component->UpdateWorldPositionScale(false);
	component->ChangedParentRotation();
	component->ParentChanged();
}

bool QSceneComponent::ForgetComponent(QSceneComponent* component)
{
	if (!component->IsValid() || component->Inseparable)
		return false;

	if (ListComponents.Remove(component))
	{
		component->ParentActorComponent = nullptr;
		component->ParentChanged();
		return true;
	}
	else
		return false;
}

QActor* QSceneComponent::GetActor(QSceneComponent* component)
{
	QActor* act = nullptr;
	QSceneComponent* tem = component;
	while (true)
	{
		if (!tem->IsValid())
			return nullptr;

		if (tem->IRootComponent)
			return tem->ParentActor;
		else
			tem = tem->ParentActorComponent;
	}
}

QWorld* QSceneComponent::GetWorld()
{
	QActor* actor = GetActor();
	if (actor->IsValid() && actor->GetWorld()->IsValid())
		return actor->GetWorld();	

	return nullptr;
}
