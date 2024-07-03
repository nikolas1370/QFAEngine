#pragma once
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Render/Pipline/Pipline.h>
#include <functional>
#include <thread>
class QSceneComponent;
class QFAContentManager;
class QFAWindow;

class QFAEXPORT QFAOverlord
{
	static bool Life;
	static bool isInit;


	static int DefaultWidth;
	static int DefaultHeight;


	static float FrameCount;
	static float FrameTime; // 1000 / frameCount
	static bool FpsLock;
	static bool ShdowFpsInConsole;

	static std::function<void()> FrameStarted;
	static std::function<void()> FrameEnded;

	static std::thread::id MainThreadId;
public:
	/*
		start processing world 
		call one time

		return false if initialization error or not init
	*/
	static bool StartLife();
	static void EndLife();

	/*
		if createWindow == false
			need create new window before QFAText::LoadFont

		cm == class based on QFAContentManager, game code can get resources in editor manager
	*/
	static bool Init(std::vector<QFAVKPipeline::SShaderData> shaderData, bool createWindow, std::function<void()> frameStarted, std::function<void()> frameEnded, QFAContentManager* cm);


	static void SetLimitFpsCount(float framesCount);
	

	static float GetLimitFpsCount();

	static void EnableFpsLock(bool enable);
	

	static bool GetFpsLock();

	static void SetShdowFpsInConsole(bool enable);

	static std::thread::id GetMainThreadId();
private:

	static void MainLoop();
};

