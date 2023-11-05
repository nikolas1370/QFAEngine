 // https://learnopengl.com/Introduction


/*



Linear Algebra! Lots of lots of Linear Algebra!

Here are just classes and example situations where you need them

    Vector - position, velocity, normals
    Matrix - transformations
    Quaternion - rotations (great for bone animations)
    Ray - projectile collision detection
    Plane - projectile collision detection
    Frustum - render culling
    Sphere - render culling, fast collision test
    Axis-Align Bounding Box - culling, collision tests, spacial partitioning
    Oriented Bounding Box - collision tests
    Convex Hull - collision, spacial partitioning
    etc.


    http://www.essentialmath.com/
*/



#include "Math/Vector.h"
//#include <Object/Mesh/Mesh.h> hidden

#pragma comment(lib, "opengl32.lib") 



#pragma comment(lib, "glfw3.lib")


#pragma comment(lib, "User32.lib") // for glfw
#pragma comment(lib, "Gdi32.lib") // for glfw
#pragma comment(lib, "Shell32.lib") // for glfw

#pragma comment(lib, "glew32s.lib") // for glfw 
//glew32s 's' mean static

/*

якшо против часової стрілки закручені вершини то це лицем а наобороо т то це дупой і так растеризатор робить culing


glEnable(GL_depth_test))
glDepthFun(GL_LESS)


glPolygonMode(GL_FRONT_AND_BACK, LineMode ? GL_LINE : GL_FILL)// GL_LINE draw line not triangle : GL_FILL  fill triangle
if(LineMode)
{
    glDisable(GL_depth_test);
    glDisable(GL_CULL_FACE);// виключить шоб буро видно ребра за тими полігонами які перед ним
}
else 
{
    glEnable(GL_CULL_FACE)
    glEnable(GL_depth_test)) draw box without артифактів
}

uniform object buffer  ната ця концепція
*/

/*

Vector Down

3D vector Unreal down direction constant (0,0,-1)
*/

//#include "Object.h"

#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
//#include <Object/Mesh/StaticMesh.h>
 
//#include "Texture.h"


#include <stdio.h>

#include <Math/Vector2D.h>

/*
#define 	GLFW_MOD_SHIFT   0x0001
    If this bit is set one or more Shift keys were held down. More...

#define 	GLFW_MOD_CONTROL   0x0002
    If this bit is set one or more Control keys were held down. More...

#define 	GLFW_MOD_ALT   0x0004
    If this bit is set one or more Alt keys were held down. More...

#define 	GLFW_MOD_SUPER   0x0008
    If this bit is set one or more Super keys were held down. More...

#define 	GLFW_MOD_CAPS_LOCK   0x0010
    If this bit is set the Caps Lock key is enabled. More...

#define 	GLFW_MOD_NUM_LOCK   0x0020
    If this bit is set the Num Lock key is enabled. More...
*/

FVector moveCam;// 
FVector rotCam;

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


float sphere_ray_intersect(FVector ray_start, FVector ray_direction, FVector sphere_centr, float soher_radius)
{// https://www.youtube.com/watch?v=ebzlMOw79Yw
    // 5:55
    glm::vec3 s = (ray_start - sphere_centr).GetGLMVector();
    float b = glm::dot(ray_start.GetGLMVector(), ray_direction.GetGLMVector());
    float c = glm::dot(s, s) - soher_radius * soher_radius;
    float h = b * b - c;
    if (h < 0.00001)
        return 1000; // no intersection

    h = sqrtf(h);
    float t = -b - h;// t is length ray
    return std::max(t, 0.0f);
}


struct Ray
{
    glm::vec3 dir;
    glm::vec3 org;
};


// t = ray len
bool loxGlobal(Ray r, float &t, glm::vec3 lb, glm::vec3 rt)
{
    glm::vec3 dirfrac; // move in Ray
    // r.dir is unit direction vector of ray
    dirfrac.x = 1.0f / r.dir.x;
    dirfrac.y = 1.0f / r.dir.y;
    dirfrac.z = 1.0f / r.dir.z;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    float t1 = (lb.x - r.org.x) * dirfrac.x;
    float t2 = (rt.x - r.org.x) * dirfrac.x;
    float t3 = (lb.y - r.org.y) * dirfrac.y;
    float t4 = (rt.y - r.org.y) * dirfrac.y;
    float t5 = (lb.z - r.org.z) * dirfrac.z;
    float t6 = (rt.z - r.org.z) * dirfrac.z;
    
    float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
    float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        std::cout << "false 1" << std::endl;
        t = tmax;
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        std::cout << "false 2" << std::endl;
        t = tmax;
        return false;
    }

    //t = tmin;  whit out in box check
    // whit check
    //I have added to @zacharmarz answer to handle when the ray origin is inside of the AABB. In this case tmin will be negative and behind the ray so tmax is the first intersection between the ray and AABB.
    // if tmin < 0 then the ray origin is inside of the AABB and tmin is behind the start of the ray so tmax is the first intersection
    
    tmin < 0 ? t = tmax : t = tmin;//  add  bool insideHit = true return tru when ray stay in box
    return true;
}



/*/
#include <Tools/File/OBJLoader.h>

#include <Tools/File/FileLoader.h>
//#include <Render/Time.h>

#include <Object/Actor/Actor.h>
#include <Object/World/World.h>

#include <Overlord/Overlord.h>
#include <Object/Camera.h>
#include "GameCode/TestActor.h"

/*---*/





int main()
{        
    QFAOverlord::Init();
    
    QMesh* MeshA = OBJLoader::LoadModelWithAnimate("NoEngineModel/anim/dore_1.obj", 30); 
    QStaticMesh* Mesh = OBJLoader::LoadModel("NoEngineModel/quad2.obj");//(vertecis, sizeof(vertecis), indices, sizeof(indices) / sizeof(unsigned int));
    QStaticMesh* Arrow = OBJLoader::LoadModel("NoEngineModel/Arrow.obj");
    Arrow->Name = "Arrow";

    /*
    * 
    * 
    * remove after re du loader for it
    * 
    */
    //Mesh->SetMesh(MeshA->Mf);
//    Render render;

        
    QWorld* mainWorld = new QWorld();
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


    mainWorld->AddActor(animActor);    
    mainWorld->AddActor(firstActor);
    
    /* -----------*/
    
    /*
    
        QStaticMesh FloorF = *Mesh;
    FloorF.SetScale(FVector(1, 80, 80));
    FloorF.SetPosition(FVector(30, 0, 0));
    //floorActor->SetActorPosition(FVector(30, 0, 0));

    floorActor->SetRootComponent(&FloorF);
    mainWorld->AddActor(floorActor);
    */

    QActor* thirdActor = new QActor();
    thirdActor->Name = "Atory";
    mainWorld->AddActor(thirdActor);
    thirdActor->SetActorPosition(FVector(30, 0, 0));

    
    //
    /*
    not work  GetCopyMesh
    */
    //QStaticMesh* qwd = Mesh->GetCopyMesh();    
    QStaticMesh qwd = *Mesh;
    qwd.Name = "Main na";
    thirdActor->SetRootComponent(&qwd);

    

    QStaticMesh qwdC = *Mesh;
    
    
    Arrow->SetRelativePosition(FVector(0, 7, 0));
    
    
    qwd.AttachComponent(&qwdC);    

    
    qwdC.SetRelativePosition(FVector(0, 0, 5));
    qwdC.AttachComponent(Arrow);    


    
    thirdActor->SetActorRotation(FVector(90, 0, 0));
    qwdC.SetRotation(FVector(45, 0, 0));
    //qwdC.SetScale(1);
    thirdActor->SetActorScale(1.0);
    Arrow->SetRotation(FVector(90, 0, 0));
    Arrow->SetScale(2); 

    //qwdC.SetWorldPosition(FVector(40, 0, 3));
    qwdC.SetLocalPosition(FVector(10, 0, 3));
    qwdC.Name = "after main";
    qwdC.SetScale(2);

    


    
    Arrow->SetWorldPosition(FVector(30, -3, 0));    
    //row->SetLocalPosition(FVector(0, -3, 0));
    qwd.SetScale(0.5);
    Arrow->SetScale(2); 
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
    mainWorld->AddActor(floorActor);



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

    
    


    //glfwSetKeyCallback(window, key_callback);
    
    //glfwSetMouseButtonCallback(window, mouse_callback);

    ATestActor* TA = new ATestActor;
    mainWorld->AddActor(TA);

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
    */
    /*
    GLFW_MOUSE_BUTTON_LEFT
    GLFW_MOUSE_BUTTON_RIGHT
    GLFW_MOUSE_BUTTON_MIDDLE
    */
}