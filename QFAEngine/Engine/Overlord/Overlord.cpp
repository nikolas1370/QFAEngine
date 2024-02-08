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



void QFAOverlord::EndLife()
{
    Life = false;    
    
    QFAFile::EndLife();
    QFAText::EndLife();
    //QFAVKBuffer::EndLife();  nee dele all buffer and image before call
    glfwTerminate(); 
    
}

bool QFAOverlord::Init(std::vector<QFAVKPipeline::SShaderData> shaderData)
{
    QTime::Init();    
    QFAVKPipeline::SetShaderData(shaderData);

    Window = new QFAWindow(DefaultWidth, DefaultHeight, "QFA");    
    QFAInput::Init(Window->glfWindow);

    isInit = true;
    return true;
}


void QFAOverlord::MainLoop()
{
    while (!Window->ShouldClose() && Life)
    {
        QTime::CalcDeltaTime();        
        glfwPollEvents();
        QFAInput::NewFrame((float)QTime::GetDeltaTime());        
        QWorld::ProcessTicks();
        QFAWindow::RenderWindow();
    }
    

    if (Life)
        QFAOverlord::EndLife();
}

