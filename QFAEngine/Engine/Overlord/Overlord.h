#pragma once
#include <Object/World/World.h>
#include <Object/Camera.h>
class QSceneComponent;
struct GLFWwindow;

class QFAOverlord 
{
	friend void QWorld::Activate();
	friend void Camera::Activate();

	

	static QWorld* CurentWorld;
	static Camera* CurentCamera;
	static bool Life;
	static bool isInit;

	static GLFWwindow* Window;


	static void RenderWorld();

	static void ProcessComponent(QSceneComponent* component);
	static void ProcessComponentShadow(QSceneComponent* component);
	static void ProcessTick();

	// call only from QWorld.Activate()
	static void SetWorld(QWorld* world); // Activate
	// call only from QCameraComponent.Activate()
	static void SetCamera(Camera* camera);
public:
	/*
		start processing world 
		call one time

		return false if initialization error or not init
	*/
	static bool StartLife();
	static void EndLife();

	/*
	return false error
	*/
	static bool Init();



	inline static QWorld* GetCurentWorld()
	{
		return QFAOverlord::CurentWorld;
	}
	

private:

	static void MainLoop();
};

