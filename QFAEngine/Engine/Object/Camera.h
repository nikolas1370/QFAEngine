#pragma once
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
class Render;

class Camera : public QSceneComponent
{
	// Field of view 
	float Fov = 90;
	float ViewDistance = 1000;
	FVector CameraRotation;
	glm::mat4 cameraRotationMatrex;	
	friend Render;
	/*world opengl position */
	inline FVector GetOpenGLPosition()// 
	{
		
		return WorldPosition;
	}
public:

	Camera(){}
	Camera(float fov, float viewDistance);

	void SetRotation(const FVector rotation) final;
	FVector GetRotation() const final;

	 

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

