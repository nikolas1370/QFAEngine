#include "Overlord.h"
#include <Object/World/World.h>
#include <Render/Render.h>
#include <Object/Camera.h>
#include <Render/Time.h> // remove in Overlord folder
#include <Render/Render.h> // remove in Overlord folder
#include <Object/Actor/Actor.h>
#include <Tools/Debug/OpenGlStuff.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Input/Input.h>

QWorld* QFAOverlord::CurentWorld = nullptr;
bool QFAOverlord::Life = false;
bool QFAOverlord::isInit = false; 
GLFWwindow* QFAOverlord::Window = nullptr;
Camera* QFAOverlord::CurentCamera = nullptr;


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
    glfwDestroyWindow(Window);
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

    Window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
    QFAInput::Init(Window);
    if (!Window)
    {
        glfwTerminate();
        ASSERT(false);
        
        return false;
    }

    glfwMakeContextCurrent(Window);

    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        ASSERT(false);
        return false;
    }

    QFARender::Init();
    QFARender::SetWindow(Window);
    std::cout << "openGL VERSION " << " " << glGetString(GL_VERSION) << std::endl;
    isInit = true;
    return true;
}

void QFAOverlord::SetWorld(QWorld* world)
{
	if (CurentWorld == world)
		return;
		
    CurentWorld = world;
}
void QFAOverlord::SetCamera(Camera* camera)
{
    QFARender::SetCamera(camera);
    CurentCamera = camera;
}
// Active
void QFAOverlord::MainLoop()
{
    while (!glfwWindowShouldClose(Window))
    {
        QTime::CalcDeltaTime();
        glfwPollEvents();
        ProcessTick();
        RenderWorld();
    }
}


void QFAOverlord::RenderWorld()
{
    if (!CurentWorld->IsValid() || !CurentCamera->IsValid())
    {
        std::cout << "QFAOverlord::MainLoop camera/world not set" << std::endl;
        QFARender::EndFrame(true);
        return;
    }
    else if (!CurentWorld->IsActive || !CurentCamera->IsActive)
    {
        QFARender::EndFrame(true);
        return;
    }

    QDirectionLight* DL = CurentWorld->GetDirectionDight();
    DL->StartFrame();
    if (DL->GetCastShadow())
        for (unsigned int i = 0; i < CurentWorld->Actors.Length(); i++)
            if (CurentWorld->Actors[i]->RootComponent->IsValid())
                ProcessComponentShadow(CurentWorld->Actors[i]->RootComponent);

    QFARender::StartFrame();

    for (unsigned int i = 0; i < CurentWorld->Actors.Length(); i++)
        if (CurentWorld->Actors[i]->RootComponent->IsValid())
            ProcessComponent(CurentWorld->Actors[i]->RootComponent);

    QFARender::EndFrame();
}

void QFAOverlord::ProcessComponent(QSceneComponent* component)
{
    if (!component->IsValid())
        return;

    if (QMeshBaseComponent* mesh = dynamic_cast<QMeshBaseComponent*>(component))
        QFARender::DrawMesh(mesh);

    for (unsigned int i = 0; i < component->ListComponents.Length(); i++)
        if (component->ListComponents[i]->IsValid())
            ProcessComponent(component->ListComponents[i]);
}

void QFAOverlord::ProcessComponentShadow(QSceneComponent* component)
{
    if (!component->IsValid())
        return;

    if (QMeshBaseComponent* mesh = dynamic_cast<QMeshBaseComponent*>(component))
        if (mesh->GetCastShadow())
            QFARender::DrawMeshShadow(mesh);

    for (unsigned int i = 0; i < component->ListComponents.Length(); i++)
        if (component->ListComponents[i]->IsValid())
            ProcessComponent(component->ListComponents[i]);
}

void QFAOverlord::ProcessTick()
{
    if (!CurentWorld->IsValid())
        return;

    float delta = (float)QTime::GetDeltaTime();
    for (unsigned int i = 0; i < CurentWorld->Actors.Length(); i++)
    {
        if (CurentWorld->Actors[i]->IsValid() && CurentWorld->Actors[i]->CanTick)
        {
            CurentWorld->Actors[i]->Tick(delta);          
            ProcessSceneComponentTick(CurentWorld->Actors[i]->RootComponent);
        }
    }
}

void QFAOverlord::ProcessSceneComponentTick(QSceneComponent* component)
{
    if (!component->IsValid())
        return;

    float delta = (float)QTime::GetDeltaTime();
    if (component->CanTick)
        component->TickComponent(delta);

    for (unsigned int i = 0; i < component->ListComponents.Length(); i++)
        if (component->ListComponents[i]->IsValid())
            ProcessSceneComponentTick(component->ListComponents[i]);
}
