#include "pch.h"
#include "Camera.h"
#include <Overlord/Overlord.h>
#include <EngineStuff/Window/Viewport.h>
#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
void QCameraComponent::Activate()
{
	if (!IsValid())
		return;

	if(Viewport)
		Viewport->CameraChangeParameter(1);

	IsActive = true;	
}

void QCameraComponent::Deactivate()
{
	if (!IsValid())
		return;

	if (Viewport)
		Viewport->CameraChangeParameter(2);

	IsActive = false;
}

QCameraComponent::QCameraComponent(float fov, float viewDistance)
{
	Fov = fov;
	ViewDistance = viewDistance;
	NeedUpdateMatrix = true;
}



void QCameraComponent::UpdateModelMatrix()
{
	FinallyRotation = Rotation;
	if (!IsRootComponent())
	{
		QSceneComponent* parent = GetParentComponent();		
		while (true)// process rotation camera
		{
			if (!parent->IsValid())
				break;

			FinallyRotation -= parent->GetRotation();
			parent = parent->GetParentComponent();
		}
	}

	cameraRotationMatrex = Math::DefauldMatrix3;
	cameraRotationMatrex = Math::rotateMatrix3(cameraRotationMatrex, glm::radians(FinallyRotation.X), glm::vec3(0.0f, 0.0f, 1.0f));
	cameraRotationMatrex = Math::rotateMatrix3(cameraRotationMatrex, glm::radians((FinallyRotation.Y) * -1), glm::vec3(1.f, 0.f, 0.f));
	cameraRotationMatrex = Math::rotateMatrix3(cameraRotationMatrex, glm::radians((FinallyRotation.Z)), glm::vec3(0.0f, -1.0f, 0.0f));
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

