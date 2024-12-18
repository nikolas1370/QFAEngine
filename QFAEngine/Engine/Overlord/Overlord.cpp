﻿#include "pch.h"
#include "Overlord.h"
#include <EngineStuff/Window/EngineWindow.h>
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Overlord/Time.h> // remove in Overlord folder
#include <Object/Actor/Actor.h>

#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>

#include <Input/Input.h>
#include <UI/Text.h>
#include <Tools/File/FileSystem.h>
#include <EngineStuff/VulkanSuff.h>
#include <EngineStuff/vk/LogicalDevice.h>
#include <GLFW/glfw3.h>
#include <Overlord/ContentManager.h>
#include <Object/Class.h>

#pragma comment(lib, "Winmm.lib")  // for timeBeginPeriod , timeEndPeriod

bool QFAOverlord::Life = false;
bool QFAOverlord::isInit = false; 
bool QFAOverlord::ShdowFpsInConsole = true;


int QFAOverlord::DefaultWidth = 1000;
int QFAOverlord::DefaultHeight = 600;

float QFAOverlord::FrameCount = 60.0f;
float QFAOverlord::FrameTime = 1000 / FrameCount;
bool QFAOverlord::FpsLock = true;

std::function<void()> QFAOverlord::FrameStarted;
std::function<void()> QFAOverlord::FrameEnded;

std::thread::id QFAOverlord::MainThreadId;

#include <Audio/Audio.h>

bool QFAOverlord::StartLife()
{
	if (Life || !isInit)
		return false;	

    MainThreadId = std::this_thread::get_id();    
    if (QFAText::Fonts.size() == 0)
        stopExecute("before call StartLife need add font, for it call ELoadFontResult LoadFont");
    
	Life = true;
    std::cout << "Engine load time " << (QTime::GetTime() / 10000) << '\n';
    QFAEngineClassInstance::InitClassesEngine();
    
	QFAOverlord::MainLoop();
	return true;
}

bool QFAOverlord::Init(std::vector<QFAVKPipeline::SShaderData> shaderData, bool createWindow, std::function<void()> frameStarted, std::function<void()> frameEnded)
{
    if (isInit)
        return false;

    FrameStarted = frameStarted;
    FrameEnded = frameEnded;

    QTime::Init();    
    QFAUIUnit::Init();
    QFAVKPipeline::SetShaderData(shaderData);

    if(createWindow)
        QFAEngineWindow::CreateEngineWindow(DefaultWidth, DefaultHeight, "QFAEditor");

    timecaps_tag ptc{};
    timeGetDevCaps(
        &ptc,
        sizeof(LPTIMECAPS)
    );// before window 10 version 2004 timeBeginPeriod use for whole system
    timeBeginPeriod(ptc.wPeriodMin); // for Sleep

    isInit = true;
    return true;
}

void QFAOverlord::SetLimitFpsCount(float framesCount)
{
    if (framesCount > 0)
    {
        FrameCount = framesCount;
        FrameTime = 1000 / FrameCount;
    }
}

float QFAOverlord::GetLimitFpsCount()
{
    return FrameCount;
}

void QFAOverlord::EnableFpsLock(bool enable)
{
    FpsLock = enable;
}

inline bool QFAOverlord::GetFpsLock()
{
    return FpsLock;
}

void QFAOverlord::SetShdowFpsInConsole(bool enable)
{
    ShdowFpsInConsole = enable;
}

std::thread::id QFAOverlord::GetMainThreadId()
{
    return MainThreadId;
}


void QFAOverlord::MainLoop()
{
    static int count = 0;
    static double deltaAcum = 0;
    float timePassedAcum = 0;

    while (true)
    {          
        if (FrameStarted)
            FrameStarted();
   
        auto t = QTime::GetSystemTime();
        QTime::CalcDeltaTime();
        glfwPollEvents();
        
        QFAInput::NewFrame((float)QTime::GetDeltaTime());
        QFAEngineWindow::ProcessUIEvent();
        QWorld::ProcessTicks();
        
        vkQueueWaitIdle(QFAVKLogicalDevice::GetGraphicsQueue());
        if (CheckWindow()) // window and uiUnit need delete after all be rendered and processed
            return;

        QFAUIUnit::FreeUnits();
        QFAVKBuffer::ProcessTaskFromOtherThread();
        QFAVKBuffer::DeleteNotNeedBuffer();
        
        QFAEngineWindow::CheckIfNeedResizeWindows();
        QFAEngineWindow::ProcessGetMeshId();
        
        QFAEngineWindow::RenderWindows();
        
        count++;
        deltaAcum += QTime::GetDeltaTime();
        float timePassed = (float)(QTime::GetSystemTime() - t) / 10000.0f;
        timePassedAcum += timePassed;

        if (ShdowFpsInConsole && deltaAcum >= 1.0)
        {            
            std::cout << "fps " << count <<" deltatime " << 1.0 / (double)count << " " << timePassedAcum / count << "\n";
            timePassedAcum = 0;
            count = 0;
            deltaAcum = 0.0;
        }

        if (FrameEnded)
            FrameEnded();

        if (FpsLock && timePassed < FrameTime)
            QFASleep(FrameTime - timePassed);
    }
}

bool QFAOverlord::CheckWindow()
{
    for (int i = QFAEngineWindow::Windows.size() - 1; i >= 0; i--)
    {
        if (QFAEngineWindow::Windows[i]->ShouldClose())
        {
            if (i == 0)
            {
                QFAOverlord::EndLife();
                return true;
            }

            QFAEngineWindow::Windows[i]->~QFAEngineWindow();
            free(QFAEngineWindow::Windows[i]);
            QFAEngineWindow::Windows.erase(QFAEngineWindow::Windows.begin() + i);
        }
    }

    return false;
}


void QFAOverlord::EndLife()
{
    vkQueueWaitIdle(QFAVKLogicalDevice::GetGraphicsQueue());
    Life = false;

    for (size_t i = 0; i < QFAEngineWindow::Windows.size(); i++)
        if (QFAEngineWindow::Windows[i]->RegularWindow)
            delete QFAEngineWindow::Windows[i];

    QFAFile::EndLife();
    QFAText::EndLife();
    //QFAVKBuffer::EndLife();  nee dele all buffer and image before call
    glfwTerminate();

    timecaps_tag ptc{};
    timeGetDevCaps(
        &ptc,
        sizeof(LPTIMECAPS)
    );
    timeEndPeriod(ptc.wPeriodMin); // need call after timeBeginPeriod
}