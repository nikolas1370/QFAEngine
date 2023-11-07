#include "SceneComponent.h"
#include <Object/Actor/Actor.h>


QSceneComponent::QSceneComponent()
{
	InWorldComponent = true;
}

QSceneComponent::~QSceneComponent()
{
	for (unsigned int i = 0; i < ListComponents.Length(); i++)
		ListComponents[i]->Destroy();
}

void QSceneComponent::ChangeWorldPosition(const FVector position)
{
	if (IRootComponent)
	{
		ParentActor->Position = position;
		RelativePosition = 0;
		AccumulateScale = FVector(1);
		WorldPosition = ToOpenglCoordinate(position);
		if (ForRender)
			UpdateModelMatrix(true);
		for (unsigned int i = 0; i < ListComponents.Length(); i++)
			if (ListComponents[i]->IsValid())
				ListComponents[i]->UpdateWorldPositionScale(true);
	}
	else if (ParentActorComponent->IsValid())
	{

		WorldPosition = ToOpenglCoordinate(position);
		RelativePosition = position - ParentActorComponent->GetWorldPosition();
		RelativePosition = RelativePosition.ReversScale(AccumulateScale);
		//invers RotationMatrix rotate finally position to pure relative position(pure = not scale and rotate)
		RelativePosition = FromOpenglCoordinate(FVector(glm::inverse(ParentActorComponent->RotationMatrix) * ToOpenglCoordinate(RelativePosition).GetGLMVector()));
		if(ForRender)
			UpdateModelMatrix(true);

		for (size_t i = 0; i < ListComponents.Length(); i++)
			if (ListComponents[i]->IsValid())
				ListComponents[i]->UpdateWorldPositionScale(true);
	}
}

void QSceneComponent::ChangeLocalPosition(const FVector position)
{
	if (IRootComponent || !ParentActorComponent->IsValid())
		return;

	QActor* temA = GetActor();
	if (!temA->IsValid())
		return;
	AccumulateScale = ParentActorComponent->AccumulateScale * ParentActorComponent->Scale;
	WorldPosition = ToOpenglCoordinate(position + temA->GetActorPosition());
	RelativePosition = FromOpenglCoordinate(WorldPosition - ParentActorComponent->WorldPosition);
	RelativePosition = RelativePosition.ReversScale(ParentActorComponent->AccumulateScale * ParentActorComponent->Scale);
	//invers RotationMatrix rotate finally position to pure relative position(pure = not scale and rotate)
	RelativePosition = FromOpenglCoordinate(FVector(glm::inverse(ParentActorComponent->RotationMatrix) * ToOpenglCoordinate(RelativePosition).GetGLMVector()));

	if (ForRender)
		UpdateModelMatrix(true);

	for (size_t i = 0; i < ListComponents.Length(); i++)
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
}


FVector QSceneComponent::GetLocalPosition()
{	
	QActor* temA = GetActor();
	if (!temA->IsValid())
		return FVector(0);

	return GetWorldPosition() - temA->GetActorPosition();	
}


void QSceneComponent::ChangedParentRotation()
{// rotate component center(relative position) around parent whit parent rotateMatrix
	glm::vec3 ass = ParentActorComponent->RotationMatrix * ToOpenglCoordinate(GetRelativeScalePosition()).GetGLMVector();	
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
	if (ForRender)
		UpdateModelMatrix(false);

	for (size_t i = 0; i < ListComponents.Length(); i++)
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
		if (ForRender)
			UpdateModelMatrix(false);

		for (size_t i = 0; i < ListComponents.Length(); i++)
			if (ListComponents[i]->IsValid())
				ListComponents[i]->ChangedParentRotation();
	}
	else if (ParentActorComponent->IsValid())
		ChangedParentRotation();
}

void QSceneComponent::UpdateWorldPositionScale(bool onlyPosition)
{
	AccumulateScale = ParentActorComponent->AccumulateScale * ParentActorComponent->Scale;
	glm::vec3 ass = ParentActorComponent->RotationMatrix * ToOpenglCoordinate(GetRelativeScalePosition()).GetGLMVector();
	WorldPosition = ParentActorComponent->WorldPosition + FVector(ass);
	if (ForRender)
		UpdateModelMatrix(onlyPosition);

	for (size_t i = 0; i < ListComponents.Length(); i++)
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
		if (ForRender)
			UpdateModelMatrix(false);

		for (size_t i = 0; i < ListComponents.Length(); i++)
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
	return FVector::ForwardVector;
}

FVector QSceneComponent::GetRightVector() const
{
	return FVector::RightVector;
}

FVector QSceneComponent::GetUpVector() const
{
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

void QSceneComponent::AttachComponent(QSceneComponent* component)
{
	if (!component)
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
	ListComponents.Add(component);	

	component->UpdateWorldPositionScale(false);
	component->ChangedParentRotation();
}

bool QSceneComponent::ForgetComponent(QSceneComponent* component)
{
	if (ListComponents.Remove(component))
	{
		component->ParentActorComponent = nullptr;
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


