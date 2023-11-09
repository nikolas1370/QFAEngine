#include "Camera.h"
#include <Overlord/Overlord.h>
void QCameraComponent::Activate()
{
	if (!IsValid())
		return;

	QFAOverlord::SetCamera(this);
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
}

void QCameraComponent::SetRotation(const FVector rotation)
{
	CameraRotation = rotation;	
	cameraRotationMatrex = glm::translate(glm::mat4(1), glm::vec3(0)); 
	
	cameraRotationMatrex = glm::rotate(cameraRotationMatrex, glm::radians(rotation.X), glm::vec3(0.0f, 0.0f, -1.0f));
	cameraRotationMatrex = glm::rotate(cameraRotationMatrex, glm::radians(rotation.Y * -1), glm::vec3(1.f, 0.f, 0.f));
	cameraRotationMatrex = glm::rotate(cameraRotationMatrex, glm::radians(rotation.Z), glm::vec3(0.0f, 1.0f, 0.0f));

}

FVector QCameraComponent::GetRotation() const
{
	return CameraRotation;
}

FVector QCameraComponent::GetForwardVector() const
{
	FVector fv = FVector::ForwardVector;

	fv = fv.RotateAngleAxis(CameraRotation.Y * -1 , FVector::RightVector);
	fv = fv.RotateAngleAxis(CameraRotation.Z, FVector::UpVector);
	
	return fv;
}

FVector QCameraComponent::GetRightVector() const
{
	FVector fv = FVector::RightVector;

	fv = fv.RotateAngleAxis(CameraRotation.X, FVector::ForwardVector);	
	fv = fv.RotateAngleAxis(CameraRotation.Y * -1, FVector::RightVector);
	fv = fv.RotateAngleAxis(CameraRotation.Z, FVector::UpVector);
	
	return fv;
}

FVector QCameraComponent::GetUpVector() const
{
	FVector fv = FVector::UpVector;

	
	fv = fv.RotateAngleAxis(CameraRotation.X, FVector::ForwardVector);
	fv = fv.RotateAngleAxis(CameraRotation.Y * -1, FVector::RightVector);	
	fv = fv.RotateAngleAxis(CameraRotation.Z, FVector::UpVector);

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