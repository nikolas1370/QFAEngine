#include "Overlord.h"
#include <Render/Window/Window.h>
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Render/Time.h> // remove in Overlord folder
#include <Object/Actor/Actor.h>

#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>

#include <Input/Input.h>



bool QFAOverlord::Life = false;
bool QFAOverlord::isInit = false; 
QFAWindow* QFAOverlord::Window = nullptr;
QCameraComponent* QFAOverlord::CurentCamera = nullptr;

int QFAOverlord::DefaultWidth = 600;
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
    glfwTerminate();
}

bool QFAOverlord::Init()
{
    if (!glfwInit())
    {
        ASSERT(false);
        return false;
    }
    QTime::Init();    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    Window = new QFAWindow(DefaultWidth, DefaultHeight, "QFA");    
    QFAInput::Init(Window->glfWindow);

    std::cout << "openGL VERSION " << " " << glGetString(GL_VERSION) << std::endl;
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

