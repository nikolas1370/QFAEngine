#include "Overlord.h"
#include <Render/Window/Window.h>
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Render/Time.h> // remove in Overlord folder
#include <Object/Actor/Actor.h>

#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>

#include <Input/Input.h>
#include <Render/UI/Text.h>
#include <Tools/File/FileSystem.h>
#include <Windows.h>
#include <Tools/VulkanSuff.h>
bool QFAOverlord::Life = false;
bool QFAOverlord::isInit = false; 
QFAWindow* QFAOverlord::Window = nullptr;
QCameraComponent* QFAOverlord::CurentCamera = nullptr;

int QFAOverlord::DefaultWidth = 1000;
int QFAOverlord::DefaultHeight = 600;

bool QFAOverlord::StartLife()
{
	if (Life || !isInit)
		return false;	

	Life = true;
	QFAOverlord::MainLoop();
	return true;
}

#pragma comment(lib, "Winmm.lib")  // for timeBeginPeriod , timeEndPeriod
bool QFAOverlord::Init(std::vector<QFAVKPipeline::SShaderData> shaderData)
{
    QTime::Init();    
    QFAVKPipeline::SetShaderData(shaderData);

    Window = new QFAWindow(DefaultWidth, DefaultHeight, "QFA");    
    QFAInput::Init(Window->glfWindow);

    timecaps_tag ptc{};
    timeGetDevCaps(
        &ptc,
        sizeof(LPTIMECAPS)
    );// before window 10 version 2004 timeBeginPeriod use for whole system
    timeBeginPeriod(ptc.wPeriodMin); // for Sleep

    isInit = true;
    return true;
}

void QFAOverlord::EndLife()
{
    Life = false;

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




void QFAOverlord::MainLoop()
{
    const float frameCount = 60.0f;
    const float frameTime = 1000 / frameCount;
    bool limit = true;

    while (!Window->ShouldClose() && Life)
    {
        auto t = QTime::GetSystemTime();
        QTime::CalcDeltaTime();
        glfwPollEvents();
        QFAInput::NewFrame((float)QTime::GetDeltaTime());
        QFAWindow::ProcessUIEvent();
        QWorld::ProcessTicks();
        QFAWindow::RenderWindow();

        float timePassed = (float)(QTime::GetSystemTime() - t) / 10000.0f;
        if(limit && timePassed < frameTime)
            QFASleep(frameTime - timePassed);
    }    


    if (Life)
        QFAOverlord::EndLife();
}

