#pragma once
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Core/EngineStuff/Pipline/Pipline.h>
#include <functional>
#include <thread>
class QSceneComponent;
class QFAContentManager;
class QFAEngineWindow;

class QFAEXPORT QFAOverlord
{
	static bool IsInit;
	static bool Life;
	static int DefaultWidth;
	static int DefaultHeight;

	static float FrameCount;
	static float FrameTime; // 1000 / frameCount
	static bool FpsLock;
	static bool ShdowFpsInConsole;

	static std::function<void()> FrameStarted;
	static std::function<void()> FrameEnded;
	static std::thread::id MainThreadId;
	static void MainLoop();
	static bool CheckWindow();

public:	
	/*
		userFontPath user defould font 
	*/
	static bool Init(QFAWindowParameter& winCreateParam, const std::u32string& engineShadersPath, const char* userFontPath);

	// init function need call before work with engine
	static bool StartLife(std::function<void()> frameStarted, std::function<void()> frameEnded);
	// free all resource and stop engine
	static void EndLife();

	static void SetLimitFpsCount(float framesCount);
	

	static float GetLimitFpsCount();

	static void EnableFpsLock(bool enable);
	

	static bool GetFpsLock();

	static void SetShdowFpsInConsole(bool enable);

	static std::thread::id GetMainThreadId();
};

