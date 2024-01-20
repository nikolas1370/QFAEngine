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
#include <Render/UI/Canvas.h>



int main()
{   
    
    

    QFAOverlord::Init();
    QStaticMesh* Mesh = QFAModelLoader::LoadModel("NoEngineModel/Arrow.fbx");
    QStaticMesh* Mesh2 = QFAModelLoader::LoadModel("NoEngineModel/quad2.obj");
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
    


    Camera.ActivateCamera(firstdViewPort);
    

 
    firstdViewPort->SetParameters(0, 0, 1, 1);    

    
    QFAText* text = new QFAText();
    QFAText* text_2 = new QFAText();
    text_2->SetText(L"Viewport 1");

    
    text_2->SetPosition(0, 0);
    text_2->SetTextSize(16);
    text_2->Color = FVector(1, 0.5, 0);
    /*-----*/



    text->SetPosition(0, 25);

    text->SetText(L"З'їли Їжака This tutorial will teach you the basics of using the Vulkan graphics and compute API. Vulkan is a new API by the Khronos group (known for OpenGL) that provides a much better abstraction of modern graphics cards. This new interface allows you to better describe what your application intends to do, which can lead to better performance and less surprising driver behavior compared to existing APIs like OpenGL and Direct3D. The ideas behind Vulkan are similar to those of Direct3D 12 and Metal, but Vulkan has the advantage of being fully cross-platform and allows you to develop for Windows, Linux and Android at the same time.");
                                
    //text->SetText(L"З'їли Їїжака їжака AaBb");
    text->Color = FVector(1, 0, 0.3f);
    text->Outline = false;
    text->OutlineColor = FVector(0.5f, 0.4f, 0.3f);
    text->Opacity = 0.9f;
    text->SetTextAlign(QFAText::ETextAlign::TACenter);

    QFAUICanvas* can = new QFAUICanvas;
    QFAUICanvas* canvas2 = new QFAUICanvas;
    canvas2->SetOverflow(QFAUIParentComponent::EOverflow::Hidden);

    firstdViewPort->AddUnit(can);
    can->AddUnit(canvas2);
    canvas2->AddUnit(text);    
    
    QFAUISlot::SCanvasSlot canvasSlot;
    canvasSlot.Height = 0.5;
    canvasSlot.Width = 0.8;
    canvasSlot.x = 0.1;
    canvasSlot.y = 0.0;
    canvas2->SetSlot(&canvasSlot);
    
    firstdViewPort->AddUnit(text_2);
    
    text_2->SetSize(600, 300);

    QFAOverlord::StartLife();

    return 0;
}

