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

#pragma comment(lib, "Winmm.lib")  // for timeBeginPeriod , timeEndPeriod

bool QFAOverlord::Life = false;
bool QFAOverlord::isInit = false; 



int QFAOverlord::DefaultWidth = 1000;
int QFAOverlord::DefaultHeight = 600;

bool QFAOverlord::StartLife()
{
	if (Life || !isInit)
		return false;	
    
    if (QFAText::Fonts.size() == 0)
        stopExecute("before call StartLife need add font, for it call ELoadFontResult LoadFont");

    std::cout << "Engine load time " << (QTime::GetTime() / 10000) << '\n';
	Life = true;
	QFAOverlord::MainLoop();
	return true;
}


bool QFAOverlord::Init(std::vector<QFAVKPipeline::SShaderData> shaderData)
{
    QTime::Init();    
    QFAVKPipeline::SetShaderData(shaderData);

    new QFAWindow(DefaultWidth, DefaultHeight, "QFA");      

    timecaps_tag ptc{};
    timeGetDevCaps(
        &ptc,
        sizeof(LPTIMECAPS)
    );// before window 10 version 2004 timeBeginPeriod use for whole system
    timeBeginPeriod(ptc.wPeriodMin); // for Sleep

    isInit = true;
    return true;
}

static int count = 0;
static double deltaAcum = 0;


void QFAOverlord::MainLoop()
{
    const float frameCount = 60.0f;
    const float frameTime = 1000 / frameCount;
    bool limit = true;
    float timePassedAcum = 0;

    while ( Life)
    {
        for (int i = QFAWindow::Windows.size() - 1; i >= 0; i--)
        {
            if (QFAWindow::Windows[i]->ShouldClose())
            {
                if (i == 0)
                {
                    QFAOverlord::EndLife();
                    return;
                }

                delete QFAWindow::Windows[i];
                QFAWindow::Windows.erase(QFAWindow::Windows.begin() + i);
            }
        }

        auto t = QTime::GetSystemTime();
        QTime::CalcDeltaTime();
        glfwPollEvents();
        QFAInput::NewFrame((float)QTime::GetDeltaTime());
        QFAWindow::ProcessUIEvent();
        QWorld::ProcessTicks();
        QFAWindow::RenderWindows();
        



        count++;
        deltaAcum += QTime::GetDeltaTime();
        float timePassed = (float)(QTime::GetSystemTime() - t) / 10000.0f;
        timePassedAcum += timePassed;

        if (deltaAcum >= 1.0)
        {            
            std::cout << "fps " << count <<" deltatime " << 1.0 / (double)count << " " << timePassedAcum / count << "\n";
            timePassedAcum = 0;
            count = 0;
            deltaAcum = 0.0;
        }

       
        if (limit && timePassed < frameTime)
            QFASleep(frameTime - timePassed);
    }
}


void QFAOverlord::EndLife()
{
    Life = false;

    for (size_t i = 0; i < QFAWindow::Windows.size(); i++)
        delete QFAWindow::Windows[i];

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





