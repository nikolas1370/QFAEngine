#include "Math/Vector.h"
#pragma comment(lib, "vulkan-1.lib")
#pragma comment(lib, "glfw3.lib")



#include <Overlord/Overlord.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Math/Vector2D.h>
#include <Object/Actor/Actor.h>

#include <Tools/File/FileLoader.h>

#include "GameCode/TestActor.h"

#include <Object/Actor/Camera/CameraEditor.h>
#include <Render/Window/Window.h>
#include <Render/Window/Viewport.h>
#include <Render/UI/Text.h>


#include <Tools/File/ModelLoader.h>

int main()
{    
    QFAOverlord::Init();

    QStaticMesh* Mesh = QFAModelLoader::LoadModel("NoEngineModel/Arrow.fbx"); // Arrow.fbx
    QStaticMesh* Mesh2 = QFAModelLoader::LoadModel("NoEngineModel/quad2.fbx");
    QStaticMesh* Mesh3 = QFAModelLoader::LoadModel("NoEngineModel/anim/dore_1.obj");

    QWorld* mainWorld = new QWorld();

    QActor* firstActor = new QActor();
    QActor* secondActor = new QActor();
    
    QActor* someActor = new QActor();
    QActor* floorActor = new QActor();
    QActor* animActor = new QActor();
    
    firstActor->SetRootComponent(Mesh2);
    
    Mesh->AttachComponent(Mesh3);
    Mesh3->SetRelativePosition(FVector(0, 0, -100));
    
    
    secondActor->SetRootComponent(Mesh);   
    secondActor->SetActorPosition(FVector(0, 0, -50));
    firstActor->SetActorPosition(FVector(0, 0, 100));
    secondActor->SetActorRotation(FVector(0, 0, 0));

    mainWorld->AddActor(firstActor);
    mainWorld->AddActor(secondActor);
    secondActor->Name = "mainactor";

    ACameraEditor Camera;    
    Camera.SetActorPosition(FVector(-1000, 0, 0)); 
    mainWorld->AddActor(&Camera);
    Camera.SetActorRotation(FVector(0, 0, 0));
    
    QFAWindow* mainWindow = QFAWindow::GetMainWindow();
    QFAViewport* firstdViewPort = mainWindow->GetViewport(0);
    
    QFAViewport* secondViewPort = new QFAViewport;
    QFAViewport* thirdViewPort = new QFAViewport;
    QFAViewport* fourthViewPort = new QFAViewport;
    
    ACameraActor* secondCamera = new ACameraActor();
    ACameraActor* thirdCamera = new  ACameraActor();
    ACameraActor* fourthCamera = new ACameraActor();
    Camera.ActivateCamera(firstdViewPort);
    
    secondCamera->ActivateCamera(secondViewPort);
    thirdCamera->ActivateCamera(thirdViewPort);
    fourthCamera->ActivateCamera(fourthViewPort);

    
    
    mainWindow->AddViewport(secondViewPort);
    mainWindow->AddViewport(thirdViewPort);
    mainWindow->AddViewport(fourthViewPort);
        
    firstdViewPort->SetParameters(0, 0, 0.5, 1);    
    secondViewPort->SetParameters(0.5f, 0.66666f,    0.5f, 0.33333f);
    thirdViewPort->SetParameters(0.5f,  0.33333f, 0.5f, 0.33333f);
    fourthViewPort->SetParameters(0.5f, 0.0f, 0.5f, 0.33333f);
        
    mainWorld->AddActor(secondCamera);
    mainWorld->AddActor(thirdCamera);
    mainWorld->AddActor(fourthCamera);
    
    
    secondCamera->SetActorPosition(FVector(-609.397, -752.392, -4.68553));
    secondCamera->SetActorRotation(FVector(0, -1.8, 49.5));

    
    thirdCamera->SetActorPosition(FVector(-645.376, 617.212, -4.68553));
    thirdCamera->SetActorRotation(FVector(0, -4.4, -51.5));

    
    fourthCamera->SetActorPosition(FVector(-909.311, -38.1247, 601.103));
    fourthCamera->SetActorRotation(FVector(0, -23.7998, -0.000142425));
    
    
    QFAText* text = new QFAText();
    QFAText* text_2 = new QFAText();
    text_2->SetText(L"Viewport 1");

    
    text_2->SetPosition(0, 0);
    text_2->SetTextSize(16);
    text_2->Color = FVector(1, 0.5, 0);
    /*-----*/

    
    
    QFAText* text2 = new QFAText();
    text2->SetText(L"Viewport 2");
    QFAText* text3 = new QFAText();
    text3->SetText(L"Viewport 3");
    QFAText* text4 = new QFAText();
    text4->SetText(L"Viewport 4");
    
    text2->Color = FVector(0, 0, 1);
    text3->Color = FVector(0, 0.5, 0);
    text4->Color = FVector(1, 0, 0);

    text2->SetPosition(0,0);
    text3->SetPosition(0,0);
    text4->SetPosition(0,0);
    
    text2->SetTextSize(25);
    text3->SetTextSize(25);
    text4->SetTextSize(25);

    
    secondViewPort->AddText(text2);
    thirdViewPort->AddText(text3);
    fourthViewPort->AddText(text4);
    

    text->SetPosition(0, 25);

    text->SetText(L"З'їли Їжака This tutorial will teach you the basics of using the Vulkan graphics and compute API. Vulkan is a new API by the Khronos group (known for OpenGL) that provides a much better abstraction of modern graphics cards. This new interface allows you to better describe what your application intends to do, which can lead to better performance and less surprising driver behavior compared to existing APIs like OpenGL and Direct3D. The ideas behind Vulkan are similar to those of Direct3D 12 and Metal, but Vulkan has the advantage of being fully cross-platform and allows you to develop for Windows, Linux and Android at the same time.");
                                
    //text->SetText(L"З'їли Їїжака їжака AaBb");
    text->Color = FVector(1, 0, 0.3f);
    text->Outline = false;
    text->OutlineColor = FVector(0.5f, 0.4f, 0.3f);
    text->Opacity = 0.9f;
    text->SetTextAlign(QFAText::ETextAlign::TACenter);

    firstdViewPort->AddText(text);



    
    firstdViewPort->AddText(text_2);
    text->SetSize(600, 300);
    text_2->SetSize(600, 300);

    QFAOverlord::StartLife();

    return 0;
}