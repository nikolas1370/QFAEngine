#include "pch.h"
#include "QFA.h"
#include <Core/EngineStuff/Window/EngineWindow.h>
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Core/Time.h> // remove in Overlord folder
#include <Object/Actor/Actor.h>

#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>

#include <Input/Input.h>
#include <UI/Text.h>
#include <Tools/File/FileSystem.h>
#include <Core/EngineStuff/VulkanSuff.h>
#include <Core/EngineStuff/vk/LogicalDevice.h>
#include <GLFW/glfw3.h>
#include <Core/ContentManager.h>
#include <Object/Class.h>
#include "EngineStuff/ShaderCompiler.h"

#pragma comment(lib, "Winmm.lib")  // for timeBeginPeriod , timeEndPeriod

bool QFAOverlord::Life = false;
bool QFAOverlord::IsInit = false;

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

bool QFAOverlord::Init(QFAWindowParameter& winCreateParam, const std::u32string& engineShadersPath, const char* userFontPath)
{
    if (IsInit)
        return false;

    QTime::Init();

    QFAShaderCompiler::ProcessShaders(engineShadersPath);

    QFAUIUnit::Init();
    QFAText::SFont* outFont;
    QFAText::LoadFont(userFontPath, outFont);

    timecaps_tag ptc{};
    timeGetDevCaps(
        &ptc,
        sizeof(LPTIMECAPS)
    );// before window 10 version 2004 timeBeginPeriod use for whole system
    timeBeginPeriod(ptc.wPeriodMin); // for Sleep

    MainThreadId = std::this_thread::get_id();

    std::cout << "Engine load time " << (QTime::GetTime() / 10000) << '\n';
    QFAEngineClassInstance::InitClassesEngine();
    IsInit = true;
    QFAEngineWindow::CreateEngineWindow(winCreateParam.width, winCreateParam.height, winCreateParam.name, winCreateParam.inCenter, winCreateParam.decorated, winCreateParam.closedFun);
    return true;
}

bool QFAOverlord::StartLife(std::function<void()> frameStarted, std::function<void()> frameEnded)
{
    if (Life)
        return false;

    FrameStarted = frameStarted;
    FrameEnded = frameEnded;
    Life = true;
    
	QFAOverlord::MainLoop();
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
    QFAShaderCompiler::EndLife();
    //QFAVKBuffer::EndLife();  nee dele all buffer and image before call
    glfwTerminate();

    timecaps_tag ptc{};
    timeGetDevCaps(
        &ptc,
        sizeof(LPTIMECAPS)
    );
    timeEndPeriod(ptc.wPeriodMin); // need call after timeBeginPeriod
}