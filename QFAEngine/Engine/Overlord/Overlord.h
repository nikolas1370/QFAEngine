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


	static float FrameCount;
	static float FrameTime; // 1000 / frameCount
	static bool FpsLock;
	static bool ShdowFpsInConsole;
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
	*/
	static bool Init(std::vector<QFAVKPipeline::SShaderData> shaderData, bool createWindow);


	static void SetLimitFpsCount(float framesCount)
	{
		if (framesCount > 0)
		{
			FrameCount = framesCount;
			FrameTime = 1000 / FrameCount;
		}
	}

	static inline float GetLimitFpsCount()
	{
		return FrameCount;
	}

	static void EnableFpsLock(bool enable)
	{
		FpsLock = enable;
	}

	static inline bool GetFpsLock()
	{
		return FpsLock;
	}

	static inline void SetShdowFpsInConsole(bool enable)
	{
		ShdowFpsInConsole = enable;
	}

private:

	static void MainLoop();
};

