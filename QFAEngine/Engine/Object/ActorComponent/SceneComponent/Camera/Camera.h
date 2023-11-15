#pragma once
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
class QFAWindow;
class QFAOverlord;
class QFAViewport;
class QWorld;
class ACameraEditor;
class QCameraComponent : public QSceneComponent
{
	friend QFAWindow;
	friend QFAOverlord;
	friend QFAViewport;
	// Field of view 
	float Fov = 90;
	float ViewDistance = 1000;

	glm::mat3 cameraRotationMatrex;	
	// parents rotation + rotation
	FVector FinallyRotation;
	
	
	QFAViewport* Viewport = nullptr;

	bool IsActive = true;
public:

	// if viewport == nullptr camera attach to default viewport
	void Activate(QFAViewport* viewport = nullptr);
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

	QWorld* GetWorld();

	inline bool GetStatus()
	{
		return IsActive;
	}
private:

};

