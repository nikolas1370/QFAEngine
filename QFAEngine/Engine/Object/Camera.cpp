#include "Camera.h"
#include <Overlord/Overlord.h>
void Camera::Activate()
{
	if (!IsValid())
		return;

	QFAOverlord::SetCamera(this);
	IsActive = true;
}

void Camera::Deactivate()
{
	if (!IsValid())
		return;

	IsActive = false;
}

Camera::Camera(float fov, float viewDistance)
{
	Fov = fov;
	ViewDistance = viewDistance;


}

void Camera::SetRotation(const FVector rotation)
{
	CameraRotation = rotation;	
	cameraRotationMatrex = glm::translate(glm::mat4(1), glm::vec3(0)); 
	
	// Rotation Relative to local coordinat  
	cameraRotationMatrex = glm::rotate(cameraRotationMatrex, glm::radians(rotation.X), glm::vec3(0.0f, 0.0f, -1.0f));
	cameraRotationMatrex = glm::rotate(cameraRotationMatrex, glm::radians(rotation.Y * -1), glm::vec3(1.f, 0.f, 0.f));
	cameraRotationMatrex = glm::rotate(cameraRotationMatrex, glm::radians(rotation.Z), glm::vec3(0.0f, 1.0f, 0.0f));

}

FVector Camera::GetRotation() const
{
	return CameraRotation;
}

// in camera not curect
FVector Camera::GetForwardVector() const
{

	FVector fv = FVector::ForwardVector;

	//RotationRelativeGlobal = true
	//fv = fv.RotateAngleAxis(CameraRotation.X, FVector::ForwardVector);	
	fv = fv.RotateAngleAxis(CameraRotation.Y * -1 , FVector::RightVector);
	fv = fv.RotateAngleAxis(CameraRotation.Z, FVector::UpVector);
	


	return fv;
}

FVector Camera::GetRightVector() const
{
	FVector fv = FVector::RightVector;

	fv = fv.RotateAngleAxis(CameraRotation.X, FVector::ForwardVector);	
	fv = fv.RotateAngleAxis(CameraRotation.Y * -1, FVector::RightVector);
	fv = fv.RotateAngleAxis(CameraRotation.Z, FVector::UpVector);
	
	return fv;
}

FVector Camera::GetUpVector() const
{
	FVector fv = FVector::UpVector;

	
	fv = fv.RotateAngleAxis(CameraRotation.X, FVector::ForwardVector);
	fv = fv.RotateAngleAxis(CameraRotation.Y * -1, FVector::RightVector);	
	fv = fv.RotateAngleAxis(CameraRotation.Z, FVector::UpVector);

	return fv;
}

void Camera::SetFov(float fov)
{
	Fov = fov;
}

void Camera::SetViewDistance(float viewDistance)
{
	ViewDistance = viewDistance;
}

