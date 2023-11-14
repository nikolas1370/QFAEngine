#pragma once
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
class QSceneComponent;
class QFAWindow;

class QFAOverlord 
{



	static QCameraComponent* CurentCamera;
	static bool Life;
	static bool isInit;

	static QFAWindow* Window;

	static int DefaultWidth;
	static int DefaultHeight;


	
	
	
	// call only from QCameraComponent.Activate()
	//static void SetCamera(QCameraComponent* camera);
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




private:

	static void MainLoop();
};

