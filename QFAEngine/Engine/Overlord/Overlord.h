#pragma once
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Render/Pipline/Pipline.h>
class QSceneComponent;
class QFAWindow;

class QFAOverlord 
{
	static bool Life;
	static bool isInit;


	static int DefaultWidth;
	static int DefaultHeight;


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
	static bool Init(std::vector<QFAVKPipeline::SShaderData> shaderData);


	

private:

	static void MainLoop();
};

