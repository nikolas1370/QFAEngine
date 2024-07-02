#pragma once
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
class QFAWindow;
class QFAOverlord;
class QFAViewport;
class QWorld;
class ACameraEditor;
class QFAEXPORT QCameraComponent : public QSceneComponent
{
	friend QFAWindow;
	friend QFAOverlord;
	friend QFAViewport;
	// Field of view 
	float Fov = 45;
	float ViewDistance = 100000;

	glm::mat3 cameraRotationMatrex;	
	// parents rotation + rotation
	FVector FinallyRotation;
	
	
	QFAViewport* Viewport = nullptr;

	bool IsActive = true;
public:

	void Activate();
	void Deactivate();
	QCameraComponent()
	{
	
	}
	QCameraComponent(float fov, float viewDistance);




	void UpdateModelMatrix() override;

	
	/* 
	
	rewrite because it is now child node


	*/
	virtual FVector GetForwardVector() const final;
	virtual FVector GetRightVector() const final;
	virtual FVector GetUpVector() const final;
	/**/


	void SetFov(float fov);
	void SetViewDistance(float viewDistance);

	inline bool GetStatus()
	{
		return IsActive;
	}
private:

};

