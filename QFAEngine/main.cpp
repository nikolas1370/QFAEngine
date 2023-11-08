 // https://learnopengl.com/Introduction
// 1896


#include "Math/Vector.h"

#pragma comment(lib, "opengl32.lib") 
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "User32.lib") // for glfw
#pragma comment(lib, "Gdi32.lib") // for glfw
#pragma comment(lib, "Shell32.lib") // for glfw

#pragma comment(lib, "glew32s.lib") // for glfw 
//glew32s 's' mean static


//#include <Object/Mesh/StaticMesh.h>
 
//#include "Texture.h"





/*
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


char mousePressed = 0;
FVector2D lastMousePos;
FVector2D mouseOffset;
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    

    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mousePressed = 1;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastMousePos.X = (float)xpos;
        lastMousePos.Y = (float)ypos;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        mousePressed = 0;   
}
/*/

#include <Overlord/Overlord.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Math/Vector2D.h>
#include <Object/Actor/Actor.h>
#include <Tools/File/OBJLoader.h>

#include <Tools/File/FileLoader.h>

#include "GameCode/TestActor.h"


int main()
{     


    /*
    MyClass* kiopu = new MyClass;
    QObject* zx = kiopu;

    
    
    std::function<void(int) > ass = [&kiopu](int l)
        {
            std::cout << l << " " << kiopu << " <-\n";
        };
    ass(1);
    */
    

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
    
    mainComponent.SetScale(1.0);
    mainComponent.SetScale(0.5);
    Arrow->SetScale(2.1); 
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
    /*
    while ()
    {
        //std::cout << QTime::GetTime() << std::endl;
        //std::cout << QTime::GetTimeSystem() << std::endl;

        //render.StartFrameShadow();
        //MeshA->StartFrame();




        
        FVector curRot = camera.GetRotation();
        if (mousePressed == 1)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            mouseOffset.X = (float)xpos - lastMousePos.X;
            mouseOffset.Y = (float)ypos - lastMousePos.Y;
            lastMousePos.X = (float)xpos;
            lastMousePos.Y = (float)ypos;

            curRot.Y -= mouseOffset.Y * 0.07f;
            curRot.Z += mouseOffset.X * 0.07f;

        }

        camera.SetWorldPosition(camera.GetWorldPosition() + (camera.GetForwardVector() * moveCam.X + camera.GetRightVector() * moveCam.Y + camera.GetUpVector() * moveCam.Z) * 0.1f);
        // recoment na
        camera.SetRotation(curRot);
     

        Ray ray;
        ray.dir = glm::vec3(0, 0, -1);
        ray.org = glm::vec3(0, 0, -10);

        glm::vec3 rt = glm::vec3(-0.5f, -0.5f, -9.5f);

        glm::vec3 lb = glm::vec3(0.5f, 0.5f, -10.5f);
        float len;

        //std::cout << (lox(ray, len, lb, rt) ? "true" : "false") << std::endl;
        loxGlobal(ray, len, lb, rt);
        //std::cout << len << std::endl;


  

    }
  ----*/  


}
FVector moveCam;
FVector rotCam;
/*
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        moveCam.X += 1;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        moveCam.X -= 1;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        moveCam.X -= 1;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        moveCam.X += 1;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        moveCam.Y -= 1;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        moveCam.Y += 1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        moveCam.Y += 1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        moveCam.Y -= 1;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        moveCam.Z += 1;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        moveCam.Z -= 1;
    }
    else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
    {
        moveCam.Z -= 1;
    }
    else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
    {
        moveCam.Z += 1;
    }
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        rotCam.Z -= 1;
    }
    else if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
    {
        rotCam.Z += 1;
    }
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        rotCam.Z += 1;
    }
    else if (key == GLFW_KEY_E && action == GLFW_RELEASE)
    {
        rotCam.Z -= 1;
    }
    /*
    else if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //rotCam.Z -= 1;
        std::cout << "lox \n";
    }
    std::cout << "lox \n";
    
    
    GLFW_MOUSE_BUTTON_LEFT
    GLFW_MOUSE_BUTTON_RIGHT
    GLFW_MOUSE_BUTTON_MIDDLE
    
}
*/

