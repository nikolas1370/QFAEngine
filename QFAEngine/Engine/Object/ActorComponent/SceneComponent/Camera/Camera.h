#pragma once
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
class QFAEngineWindow;
class QFAOverlord;
class QFAEngineViewport;
class QWorld;
class ACameraEditor;
class ACameraActor;
class QFAEXPORT QCameraComponent : public QSceneComponent
{
	QFAEngineClassIn(QCameraComponent);
	friend QFAEngineWindow;
	friend QFAOverlord;
	friend QFAEngineViewport;
	friend ACameraActor;
	// Field of view 
	float Fov = 45;
	float ViewDistance = 100000;

	glm::mat3 cameraRotationMatrex;	
	// parents rotation + rotation
	FVector FinallyRotation;
	
	
	QFAEngineViewport* Viewport = nullptr;

	bool IsActive = true;
public:

	void Activate();
	void Deactivate();
	QCameraComponent()
	{
	
	}
	QCameraComponent(float fov, float viewDistance);

	~QCameraComponent();

	void UpdateModelMatrix() override;
	
	/* 
	
	rewrite because it is now child node


	*/
	virtual FVector GetForwardVector() const final;
	virtual FVector GetRightVector() const final;
	virtual FVector GetUpVector() const final;

	void SetFov(float fov);
	void SetViewDistance(float viewDistance);
	inline float GetFov()
	{
		return Fov;
	}
	inline float GetViewDistance()
	{
		return ViewDistance;
	}


	inline bool GetStatus()
	{
		return IsActive;
	}
};

