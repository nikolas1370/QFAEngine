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
#include <Render/UI/TextInput.h>

int main()
{
    QFAEditorOverlord::Init();

   // QStaticMesh* Mesh = QFAModelLoader::LoadModel("SomeModel/Arrow.fbx");
    //QStaticMesh* Mesh2 = QFAModelLoader::LoadModel("SomeModel/quad2.obj");
    //QStaticMesh* Mesh3 = QFAModelLoader::LoadModel("SomeModel/anim/dore_1.obj");

    QWorld* mainWorld = new QWorld();

    QActor* firstActor = new QActor();
    QActor* secondActor = new QActor();

    QActor* someActor = new QActor();
    QActor* floorActor = new QActor();
    QActor* animActor = new QActor();

    //firstActor->SetRootComponent(Mesh2);

    //Mesh->AttachComponent(Mesh3);
    //Mesh3->SetRelativePosition(FVector(0, 0, -100));


    //secondActor->SetRootComponent(Mesh);
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
    text->SetTextSize(30);
    QFAText* text_2 = new QFAText();
    text_2->SetText(U"Viewport 1 supre test");


    text_2->SetPosition(0, 0);
    text_2->SetTextSize(30);    
    text_2->Color = FVector(1, 0.5, 0);
    /*-----*/

    text->SetPosition(0, 25);
    // spaece respawn
    text->SetText(U"З'їли jЇжака This tutorial will teach you the basics of using the Vulkan graphics and compute API. Vulkan is a new API by the Khronos group (known for OpenGL) that provides a much better abstraction of modern graphics cards. This new interface allows you to better describe what your application intends to do, which can lead to better performance and less surprising driver behavior compared to existing APIs like OpenGL and Direct3D. The ideas behind Vulkan are similar to those of Direct3D 12 and Metal, but Vulkan has the advantage of being fully cross-platform and allows you to develop for Windows, Linux and Android at the same time.");

    text->Color = FVector(1, 0, 0.3f);
    text->Outline = false;
    text->OutlineColor = FVector(0.5f, 0.4f, 0.3f);    
    text->SetTextAlign(QFAText::ETextAlign::TACenter);





    
    //firstdViewPort->AddUnit(text_2);

    text_2->SetSize(600, 300);



    /*-----*/
    QFAViewport* secondViewPort = new QFAViewport;
    QFAViewport* thirdViewPort = new QFAViewport;
    QFAViewport* fourthViewPort = new QFAViewport;

    ACameraActor* secondCamera = new ACameraActor();
    mainWorld->AddActor(secondCamera);
    secondCamera->SetActorPosition(FVector(-609.397f, -752.392f, -4.68553f));
    secondCamera->SetActorRotation(FVector(0.0f, -1.8f, 49.5f));
    QFAText* textViewPort2 = new QFAText();
    textViewPort2->SetText(U"ViewPort 2");
    textViewPort2->SetTextSize(29);
    textViewPort2->SetPosition(0, 0);

    secondCamera->ActivateCamera(secondViewPort);
    mainWindow->AddViewport(secondViewPort);
    secondViewPort->SetParameters(0.5f, 0.66666f, 0.5f, 0.33333f);
  



    /*---*/
    
    QFAImage* dogy = new QFAImage(U"SomeImage/6213157_0.jpg");
    QFAUIImage* UIDogy = new QFAUIImage(dogy);
    
    QFAImage* homy = new QFAImage("SomeImage/relax, Akim Kaliberda.jpg");
    QFAUIImage* UIHomy = new QFAUIImage(homy);
    
    QFAImage* Crystal = new QFAImage("SomeImage/TankWaterSpiritCrystal.png");
    QFAUIImage* UICrystal = new QFAUIImage(Crystal);

    QFAImage* home_2 = new QFAImage(U"SomeImage/Зїли їжака.png");
    QFAUIImage* UIhome_2 = new QFAUIImage(home_2);

    QFAImage* OldWell = new QFAImage(U"SomeImage/Old Water Well.png");    

    QFAUIImage* BackgroundColor = new QFAUIImage(nullptr);
    BackgroundColor->SetBackgroundColor(FVector4D(1, 1, 1, 0.5));

    QFAUIGrid* grid = new QFAUIGrid();
    grid->SetPositionType(QFAUIGrid::UPTAuto);
    grid->SetUnitWidth(0);
    grid->SetMinMax(100, 200);
    grid->SetOffsets(10, 10);

    QFAUIScroll* scroll = new QFAUIScroll;
    scroll->SetUnit(text);
    scroll->SetBackgroundImage(OldWell);
    scroll->SetBackgroundType(EBackgroundType::BTImage);


    QFAUIScroll* scrollGrid = new QFAUIScroll;
    scrollGrid->SetUnit(grid);
    scrollGrid->SetBackgroundType(EBackgroundType::BTColor);
    scrollGrid->SetBackgroundColor(FVector4D(1, 0, 0, 0.5));
    
    QFAUITextInput* textInput = new QFAUITextInput;
    textInput->SetBackgroundImage(OldWell);
    
    grid->AddUnit(UICrystal);
    grid->AddUnit(UIDogy);
    grid->AddUnit(UIHomy);
     
    grid->AddUnit(BackgroundColor); 
    //grid->AddUnit(UIhome_2); 
    grid->AddUnit(textInput);
    grid->AddUnit(scroll);
    

    secondViewPort->AddUnit(textViewPort2);
    
    

    firstdViewPort->AddUnit(scrollGrid);
    scrollGrid->SetSize(500, 250);

    grid->UnitName = "grid";
    UIDogy->UnitName = "Dogy";
    UICrystal->UnitName = "Crystal";
    UIHomy->UnitName = "Homy";
    scroll->UnitName = "scroll";
    BackgroundColor->UnitName = "BackgroundColor";
    UIhome_2->UnitName = "home_2";
    scrollGrid->UnitName = "scrollGrid";
    text->UnitName = "text in scroll";

    QFAEditorOverlord::StartLife();


    
    return 0;
}

