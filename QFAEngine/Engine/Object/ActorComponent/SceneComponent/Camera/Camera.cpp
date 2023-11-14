#include "Camera.h"
#include <Overlord/Overlord.h>
#include <Render/Window/Viewport.h>
#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
void QCameraComponent::Activate(QFAViewport* viewport)
{
	if (!IsValid())
		return;

	if (viewport)
		viewport->ChangeCamera(this);
	else if (Viewport = QFAViewport::GetDefaultViewport())
	{
		
		Viewport->ChangeCamera(this);
	}

	IsActive = true;	
}

void QCameraComponent::Deactivate()
{
	if (!IsValid())
		return;

	IsActive = false;
}

QCameraComponent::QCameraComponent(float fov, float viewDistance)
{
	Fov = fov;
	ViewDistance = viewDistance;
	NeedUpdateMatrix = true;
}



void QCameraComponent::UpdateModelMatrix(bool onlyPosition)
{
	if (onlyPosition)
		return;

	FinallyRotation = Rotation;
	if (!IsRootComponent())
	{
		QSceneComponent* parent = GetParentComponent();		
		while (true)
		{
			if (!parent->IsValid())
				break;

			FinallyRotation -= parent->GetRotation();
			parent = parent->GetParentComponent();
		}
	}

	cameraRotationMatrex = Math::DefauldMatrix3;
	cameraRotationMatrex = Math::rotateMatrix3(cameraRotationMatrex, glm::radians(FinallyRotation.X), glm::vec3(0.0f, 0.0f, -1.0f));
	cameraRotationMatrex = Math::rotateMatrix3(cameraRotationMatrex, glm::radians((FinallyRotation.Y) * -1), glm::vec3(1.f, 0.f, 0.f));
	cameraRotationMatrex = Math::rotateMatrix3(cameraRotationMatrex, glm::radians((FinallyRotation.Z)), glm::vec3(0.0f, 1.0f, 0.0f));
}

FVector QCameraComponent::GetForwardVector() const
{
	FVector fv = FVector::ForwardVector;
	
	fv = fv.RotateAngleAxis(FinallyRotation.Y * -1 , FVector::RightVector);
	fv = fv.RotateAngleAxis(FinallyRotation.Z, FVector::UpVector);
	
	return fv;
}

FVector QCameraComponent::GetRightVector() const
{
	FVector fv = FVector::RightVector;

	fv = fv.RotateAngleAxis(FinallyRotation.X, FVector::ForwardVector);
	fv = fv.RotateAngleAxis(FinallyRotation.Y * -1, FVector::RightVector);
	fv = fv.RotateAngleAxis(FinallyRotation.Z, FVector::UpVector);
	
	return fv;
}

FVector QCameraComponent::GetUpVector() const
{
	FVector fv = FVector::UpVector;

	
	fv = fv.RotateAngleAxis(FinallyRotation.X, FVector::ForwardVector);
	fv = fv.RotateAngleAxis(FinallyRotation.Y * -1, FVector::RightVector);
	fv = fv.RotateAngleAxis(FinallyRotation.Z, FVector::UpVector);

	return fv;
}

void QCameraComponent::SetFov(float fov)
{
	Fov = fov;
}

void QCameraComponent::SetViewDistance(float viewDistance)
{
	ViewDistance = viewDistance;
}

QWorld* QCameraComponent::GetWorld()
{
	QActor* actor = GetActor();
	if (actor->IsValid())
		return actor->GetWorld();
	else
		return nullptr;
}
