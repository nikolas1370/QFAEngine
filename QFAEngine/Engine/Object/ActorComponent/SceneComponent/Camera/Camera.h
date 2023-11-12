#pragma once
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
class QFARender;
class QFAOverlord;
class QCameraComponent : public QSceneComponent
{


	// Field of view 
	float Fov = 90;
	float ViewDistance = 1000;

	glm::mat3 cameraRotationMatrex;	
	// parents rotation + rotation
	FVector FinallyRotation;
	friend QFARender;
	friend QFAOverlord;
	/*world opengl position */
	
	bool IsActive = true;
public:

	void Activate();
	void Deactivate();
	QCameraComponent()
	{
		Fov = 45.0f;
		ViewDistance = 1000.0f;
	}
	QCameraComponent(float fov, float viewDistance);




	void UpdateModelMatrix(bool onlyPosition) override;

	
	/* 
	
	rewrite because it is now child node


	*/
	virtual FVector GetForwardVector() const final;
	virtual FVector GetRightVector() const final;
	virtual FVector GetUpVector() const final;
	/**/


	void SetFov(float fov);
	void SetViewDistance(float viewDistance);
private:

};

