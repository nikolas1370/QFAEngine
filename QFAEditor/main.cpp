#include "Math/Vector.h"


#include <Overlord/Overlord.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Math/Vector2D.h>
#include <Object/Actor/Actor.h>




#include <Object/Actor/Camera/CameraEditor.h>
#include <Render/Window/Window.h>
#include <Render/Window/Viewport.h>
#include <Render/UI/Text.h>


#include <Tools/File/ModelLoader.h>
#include <Render/UI/Canvas.h>
#include <Render/UI/UIImage.h>
    
#pragma comment(lib, "QFAEngine.lib")
#include <Tools/File/FilesyStem.h>


#include <Render/Time.h>

#include "Editor/EditorOverlord.h"

int main()
{
    QFAEditorOverlord::Init();

    QStaticMesh* Mesh = QFAModelLoader::LoadModel("SomeModel/Arrow.fbx");
    QStaticMesh* Mesh2 = QFAModelLoader::LoadModel("SomeModel/quad2.obj");
    QStaticMesh* Mesh3 = QFAModelLoader::LoadModel("SomeModel/anim/dore_1.obj");

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


    firstdViewPort->SetParameters(0, 0, 0.5, 1);
    //   firstdViewPort->SetParameters(0, 0, 1, 1);    


    QFAText* text = new QFAText();
    QFAText* text_2 = new QFAText();
    text_2->SetText(L"Viewport 1 supre test");


    text_2->SetPosition(0, 0);
    text_2->SetTextSize(30);    
    text_2->Color = FVector(1, 0.5, 0);
    /*-----*/

    text->SetPosition(0, 25);

    text->SetText(L"З'їли Їжака This tutorial will teach you the basics of using the Vulkan graphics and compute API. Vulkan is a new API by the Khronos group (known for OpenGL) that provides a much better abstraction of modern graphics cards. This new interface allows you to better describe what your application intends to do, which can lead to better performance and less surprising driver behavior compared to existing APIs like OpenGL and Direct3D. The ideas behind Vulkan are similar to those of Direct3D 12 and Metal, but Vulkan has the advantage of being fully cross-platform and allows you to develop for Windows, Linux and Android at the same time.");

    text->Color = FVector(1, 0, 0.3f);
    text->Outline = false;
    text->OutlineColor = FVector(0.5f, 0.4f, 0.3f);    
    text->SetTextAlign(QFAText::ETextAlign::TACenter);

    QFAUICanvas* can = new QFAUICanvas;
    QFAUICanvas* canvas2 = new QFAUICanvas;
    QFAUICanvas* canvas3 = new QFAUICanvas;
    can->SetOpacity(1);
    canvas3->SetOpacity(0.2);
    canvas2->SetOverflow(QFAUIParentComponent::EOverflow::HiddenHorizon);
    canvas3->SetOverflow(QFAUIParentComponent::EOverflow::HiddenVertical);

    firstdViewPort->AddUnit(can);
    can->AddUnit(canvas2);
    canvas2->AddUnit(canvas3);
    canvas3->AddUnit(text);

    QFAUISlot::SCanvasSlot canvasSlot;
    canvasSlot.Height = 0.5;
    canvasSlot.Width = 0.8;
    canvasSlot.x = 0.1;
    canvasSlot.y = 0.0;
    canvas2->SetSlot(&canvasSlot);

    QFAUISlot::SCanvasSlot canvasSlot2;
    canvasSlot2.Height = 1.8;
    canvasSlot2.Width = 1;
    canvasSlot2.x = -0.1;
    canvasSlot2.y = 0.0;
    canvas3->SetSlot(&canvasSlot2);

    firstdViewPort->AddUnit(text_2);

    text_2->SetSize(600, 300);



    /*-----*/
    QFAViewport* secondViewPort = new QFAViewport;
    QFAViewport* thirdViewPort = new QFAViewport;
    QFAViewport* fourthViewPort = new QFAViewport;

    ACameraActor* secondCamera = new ACameraActor();
    mainWorld->AddActor(secondCamera);
    secondCamera->SetActorPosition(FVector(-609.397, -752.392, -4.68553));
    secondCamera->SetActorRotation(FVector(0, -1.8, 49.5));
    QFAText* textViewPort2 = new QFAText();
    textViewPort2->SetText(L"ViewPort 2");
    textViewPort2->SetTextSize(30);
    textViewPort2->SetPosition(0, 0);

    secondCamera->ActivateCamera(secondViewPort);
    mainWindow->AddViewport(secondViewPort);
    secondViewPort->SetParameters(0.5f, 0.66666f, 0.5f, 0.33333f);
    secondViewPort->AddUnit(textViewPort2);


    /*---*/
    
    QFAImage* dogy = new QFAImage("SomeImage/6213157_0.jpg");
    QFAUIImage* UIDogy = new QFAUIImage(dogy);
    UIDogy->SetZIndex(-1);
    firstdViewPort->AddUnit(UIDogy);
    UIDogy->SetPosition(0, 250);
    UIDogy->SetSize(200, 200);

    QFAImage* homy = new QFAImage("SomeImage/relax, Akim Kaliberda.jpg");
    QFAUIImage* UIHomy = new QFAUIImage(homy);
    UIHomy->SetPosition(0, 400);
    UIHomy->SetSize(200, 200);
    

    QFAUISlot::SCanvasSlot imageSlot;
    imageSlot.Height = 0.25;
    imageSlot.Width = 0.25;
    imageSlot.x = 0.0;
    imageSlot.y = 0.6;
    UIHomy->SetSlot(&imageSlot);
    can->AddUnit(UIHomy);

    

    QFAEditorOverlord::StartLife();

    return 0;
}

