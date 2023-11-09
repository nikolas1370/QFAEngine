 // https://learnopengl.com/Introduction
// 2005


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


int main()
{     

    QFAOverlord::Init();


    QMesh* MeshA = OBJLoader::LoadModelWithAnimate("NoEngineModel/anim/dore_1.obj", 30); 
    QStaticMesh* Mesh = OBJLoader::LoadModel("NoEngineModel/quad2.obj");//(vertecis, sizeof(vertecis), indices, sizeof(indices) / sizeof(unsigned int));
    QStaticMesh* Arrow = OBJLoader::LoadModel("NoEngineModel/Arrow.obj");
    Arrow->Name = "Arrow";
    MeshA->Name = "animation na";
    /*
    * 
    * 
    * remove after re du loader for it
    * 
    */
    //Mesh->SetMesh(MeshA->Mf);
//    Render render;

        
    QWorld* mainWorld = new QWorld();
    mainWorld->AddActor(new ATestActor);
    //mainWorld->GetDirectionDight()->SetCastShadow(false);

    QActor* firstActor = new QActor();
    QActor* secondActor = new QActor();
    
    QActor* someActor = new QActor();
    QActor* floorActor = new QActor();
    QActor* animActor = new QActor();

    firstActor->SetRootComponent(Arrow);
    
    secondActor->SetRootComponent(Mesh);   
    

    animActor->SetRootComponent(MeshA);

    MeshA->SetWorldPosition(FVector(-17, 0, -2));
    MeshA->SetScale(FVector(1, 1, 1));
    MeshA->SetRotation(FVector(0, 0, 90));


    //mainWorld->AddActor(animActor);    
    //mainWorld->AddActor(firstActor);
    
    /* -----------*/
    
    /*
    
        QStaticMesh FloorF = *Mesh;
    FloorF.SetScale(FVector(1, 80, 80));
    FloorF.SetPosition(FVector(30, 0, 0));
    //floorActor->SetActorPosition(FVector(30, 0, 0));

    floorActor->SetRootComponent(&FloorF);
    mainWorld->AddActor(floorActor);
    */

    /*------------------------------*/
    QActor* mainActor = new QActor();
    mainActor->Name = "mainActor";
    mainWorld->AddActor(mainActor);
    mainActor->SetActorPosition(FVector(30, 0, 0));

    
    //
    /*
    not work  GetCopyMesh
    */
    //QStaticMesh* qwd = Mesh->GetCopyMesh();    
    QStaticMesh mainComponent = *Mesh;
    mainComponent.Name = "mainComponent";
    mainActor->SetRootComponent(&mainComponent);

    

    QStaticMesh secondComponent = *Mesh;
    secondComponent.Name = "secondComponent";
    
    Arrow->SetRelativePosition(FVector(0, 7, 0));
    
    
    mainComponent.AttachComponent(&secondComponent);

    
    secondComponent.SetRelativePosition(FVector(0, 0, 5));
    secondComponent.AttachComponent(Arrow);


    
    mainActor->SetActorRotation(FVector(90, 0, 0));
    secondComponent.SetRotation(FVector(45, 0, 0));
   
    mainActor->SetActorScale(1.0);
    Arrow->SetRotation(FVector(90, 0, 0));
   // Arrow->SetScale(2); 

    secondComponent.SetLocalPosition(FVector(10, 0, 3));
    //secondComponent.SetWorldPosition(FVector(40, 0, 3));
    
    
    secondComponent.SetScale(2);

    


    
    Arrow->SetWorldPosition(FVector(30, -3, 0));    
    
    mainComponent.SetScale(1.0f);
    mainComponent.SetScale(0.5f);
    Arrow->SetScale(2.1f); 
    /*-----------*/
    /*-----------*/
    /*-----------*/
    /*-----------*/
    /*-----------*/
    
    QStaticMesh Floor = *Mesh;// bottom
    //Floor.SetScale(FVector(80, 80, 1));
    //Floor.SetWorldPosition(FVector(0, 0, -10));

    

    QStaticMesh FloorT = *Mesh;
    //FloorT.SetScale(FVector(80, 80, 1));
    //FloorT.SetWorldPosition(FVector(0, 0, 80));
    

    QStaticMesh FloorF = *Mesh;
    FloorF.SetScale(FVector(1, 80, 80));
    
    floorActor->SetActorPosition(FVector(-10, 0, 0));

    floorActor->SetRootComponent(&FloorF);
    //mainWorld->AddActor(floorActor);



    QStaticMesh FloorBa = *Mesh;
    //FloorBa.SetScale(FVector(1, 80, 80));
    //FloorBa.SetWorldPosition(FVector(-80, 0, 0));


    QStaticMesh FloorR = *Mesh;
    //FloorR.SetScale(FVector(80, 1, 80));
    //FloorR.SetWorldPosition(FVector(0, 80, 0));

    QStaticMesh FloorL = *Mesh;
    //FloorL.SetScale(FVector(80, 1, 80));
    //FloorL.SetWorldPosition(FVector(0, -80, 0));

    /*-----------*/


    
    QActor* cameraActor = new QActor();
    Camera camera(45, 1000);
    cameraActor->SetRootComponent(&camera);

    camera.SetWorldPosition(FVector(0, 0, 0));
    //camera.SetRotation(FVector(-110, 370, 940)); //test
    camera.SetRotation(FVector(0, 0,  0));

    mainWorld->Activate();
    camera.Activate();
    QFAOverlord::StartLife();

    return 0;
}
