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
#include <Render/UI/Grid.h>
#include <Render/UI/Scroll.h>


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





    // recoment
    //firstdViewPort->AddUnit(text_2);

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
    
    QFAImage* dogy = new QFAImage(U"SomeImage/6213157_0.jpg");
    QFAUIImage* UIDogy = new QFAUIImage(dogy);
    
    QFAImage* homy = new QFAImage("SomeImage/relax, Akim Kaliberda.jpg");
    QFAUIImage* UIHomy = new QFAUIImage(homy);
    
    QFAImage* Crystal = new QFAImage("SomeImage/TankWaterSpiritCrystal.png");
    QFAUIImage* UICrystal = new QFAUIImage(Crystal);

    QFAImage* home_2 = new QFAImage(U"SomeImage/Зїли їжака.png");
    QFAUIImage* UIhome_2 = new QFAUIImage(home_2);


    QFAUIGrid grid;
    grid.SetPositionType(QFAUIGrid::UPTAuto);
    grid.SetUnitWidth(0);

    QFAUIScroll scroll;
    scroll.SetUnit(text);

    QFAUIScroll scrollGrid;
    scrollGrid.SetUnit(&grid);
    
    grid.AddUnit(UIDogy);
    grid.AddUnit(UICrystal);
    grid.AddUnit(UIHomy);
    grid.AddUnit(&scroll);
    grid.AddUnit(UIhome_2);
    
    grid.SetMinMax(100, 200);
    grid.SetOffsets(10, 10);


    firstdViewPort->AddUnit(&scrollGrid);
    scrollGrid.SetSize(500, 250);



    QFAEditorOverlord::StartLife();



    return 0;
}

