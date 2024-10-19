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
	friend QWorld;
	// only main camera csn hear audio
	static QCameraComponent* MainCamera;

	// Field of view 
	float Fov = 45;
	float ViewDistance = 100000;

	glm::mat3 cameraRotationMatrex;	
	// parents rotation + rotation
	FVector FinallyRotation;
	
	
	QFAEngineViewport* Viewport = nullptr;

	bool IsActive = true;
	bool CanPlayAudio = true;

	void UpdatePosition() final;
protected:
	static void ZerosMainCamera();

public:

	void Activate();
	void Deactivate();	
	// Activate == true enable audio for this camera disable for old main
	void ActivateAudio(bool Activate = true);
	QCameraComponent()
	{
	
	}
	QCameraComponent(float fov, float viewDistance);

	~QCameraComponent();

	void UpdateModelMatrix() final;
	

	FVector GetForwardVector() const final;
	FVector GetRightVector() const final;
	FVector GetUpVector() const final;

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

	static QCameraComponent* GetMainCamera();
};

