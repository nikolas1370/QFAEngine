 // https://learnopengl.com/Introduction
// 2638
/*
Q swith control between camera and some actor whit mehes
*/


#include "Math/Vector.h"

#pragma comment(lib, "opengl32.lib") 
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "User32.lib") // for glfw
#pragma comment(lib, "Gdi32.lib") // for glfw
#pragma comment(lib, "Shell32.lib") // for glfw

#pragma comment(lib, "glew32s.lib") // for glfw 
//glew32s 's' mean static

#include <Overlord/Overlord.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Math/Vector2D.h>
#include <Object/Actor/Actor.h>
#include <Tools/File/OBJLoader.h>

#include <Tools/File/FileLoader.h>

#include "GameCode/TestActor.h"

#include <Object/Actor/Camera/CameraEditor.h>
#include <Render/Window/Window.h>
#include <Render/Window/Viewport.h>
#include <Render/Text/Text.h>

int main()
{    
    QFAOverlord::Init();

    QMesh* MeshA = OBJLoader::LoadModelWithAnimate("NoEngineModel/anim/dore_1.obj", 30); 
    //QStaticMesh* Mesh = OBJLoader::LoadModel("NoEngineModel/quad2.obj");//(vertecis, sizeof(vertecis), indices, sizeof(indices) / sizeof(unsigned int));
    //QStaticMesh* Arrow = OBJLoader::LoadModel("NoEngineModel/Arrow.obj");
    //Arrow->Name = "Arrow";
    MeshA->Name = "animation na";

    QWorld* mainWorld = new QWorld();

    QActor* firstActor = new QActor();
    QActor* secondActor = new QActor();
    
    QActor* someActor = new QActor();
    QActor* floorActor = new QActor();
    QActor* animActor = new QActor();

    //firstActor->SetRootComponent(Arrow);
    
    //secondActor->SetRootComponent(Mesh);   
    

    animActor->SetRootComponent(MeshA);
    animActor->SetActorPosition(FVector(30, 0, 0));
    

    /*------------------------------*/
    QActor* mainActor = new QActor();
    mainActor->Name = "mainActor";
    //mainWorld->AddActor(mainActor);
    mainWorld->AddActor(animActor);    
    mainActor->SetActorPosition(FVector(30, 0, 0));
    
    //
    /*
    not work  GetCopyMesh
    */
    //QStaticMesh* qwd = Mesh->GetCopyMesh();    
    //QStaticMesh mainComponent = *Mesh;
    //mainComponent.Name = "mainComponent";
    //mainActor->SetRootComponent(&mainComponent);

    //QStaticMesh secondComponent = *Mesh;
    //secondComponent.Name = "secondComponent";
    
    //Arrow->SetRelativePosition(FVector(0, 7, 0));
        
    //mainComponent.AttachComponent(&secondComponent);
    /*
    secondComponent.SetRelativePosition(FVector(0, 0, 5));
    secondComponent.AttachComponent(Arrow);
   
    mainActor->SetActorScale(1.0);

    secondComponent.SetLocalPosition(FVector(10, 0, 3));
    
    
    Arrow->SetWorldPosition(FVector(30, -3, 0));    
    
    mainComponent.SetScale(0.5f);
    secondComponent.SetScale(2);
    Arrow->SetScale(2.0f); 

    secondComponent.SetRelativePosition(FVector(0, 0, 10));
    Arrow->SetRelativePosition(FVector(0, 0, -10));
    
    secondComponent.SetRotation(FVector(-14, 480, 750));

    /*-----------*/
    /*-----------*/
    /*-----------*/
    /*-----------*/
    /*-----------*/
   
    
    ACameraEditor Camera;    
    //Camera.SetActorPosition(FVector(0, -2, 0));
    mainWorld->AddActor(&Camera);
    mainActor->SetActorPosition(FVector(30, 0, 0));
    
    QFAWindow* mainWindow = QFAWindow::GetMainWindow();
    QFAViewport* firstdViewPort = mainWindow->GetViewport(0);
    /*
    QFAViewport* secondViewPort = new QFAViewport;
    QFAViewport* thirdViewPort = new QFAViewport;
    QFAViewport* fourthViewPort = new QFAViewport;
    */
    ACameraActor* secondCamera = new ACameraActor();
    ACameraActor* thirdCamera = new  ACameraActor();
    ACameraActor* fourthCamera = new ACameraActor();
    Camera.ActivateCamera(firstdViewPort);
    /*
    secondCamera->ActivateCamera(secondViewPort);
    thirdCamera->ActivateCamera(thirdViewPort);
    fourthCamera->ActivateCamera(fourthViewPort);
    */
    /*
    mainWindow->AddViewport(secondViewPort);
    mainWindow->AddViewport(thirdViewPort);
    mainWindow->AddViewport(fourthViewPort);
    */

    firstdViewPort->SetParameters(0,0, 0.5f, 0.5f);
    /*
    secondViewPort->SetParameters(0.5f, 0, 0.5f, 0.5f);
    thirdViewPort->SetParameters(0, 0.5f, 0.5f, 0.5f);
    fourthViewPort->SetParameters(0.5f, 0.5f, 0.5f, 0.5f);
    */
    

    mainWorld->AddActor(secondCamera);
    mainWorld->AddActor(thirdCamera);
    mainWorld->AddActor(fourthCamera);

    secondCamera->SetActorPosition(FVector(30, 30, 5));
    secondCamera->SetActorRotation(FVector(0, 0, -90));


    thirdCamera->SetActorPosition(FVector(60, 0, 5));
    thirdCamera->SetActorRotation(FVector(0, 0, 180));

    
    fourthCamera->SetActorPosition(FVector(16.2079f, 1.94214f, -3.84775f));
    fourthCamera->SetActorRotation(FVector(0.f, 35.9f, -10.f));
    
    QFARenderText* text = new QFARenderText();
    

    text->SetPosition(-1500, 200);
    text->SetText("AajBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz.,:;'\"\`~/|\=+-_");
    firstdViewPort->AddText(text);
    

    QFAOverlord::StartLife();

    return 0;
}
