#pragma once
#include <iostream>
#include "../../QFAEngine/Engine/Object/Object.h"
#include "../../QFAEngine/Engine/Object/Actor/Actor.h"
#include <Overlord/ContentManager.h>
#include <UI/TextInput.h>

#define InGameModule

class Test : public QObject
{
    QFAClassIn(Test)    
private:
    
public:
    Test()
    {

    }

    ~Test()
    {
        
    }
};
/*
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
class QTest_ActorMinusOne : public QActor
{
    QFAClassIn(QTest_ActorMinusOne);
    static bool Red;

private:
    QStaticMesh* mesh = nullptr;
public:
    QTest_ActorMinusOne()
    {


        
        QStaticMesh* mesh = NewObject<QStaticMesh>();
        mesh->SetMesh(QFAContentManager::GetMesh(U"Content\\Arrow.qfa"));
        //mesh = new QStaticMesh(QFAContentManager::GetMesh(U"Content\\untitled.qfa"));
        if(!mesh)
            stopExecute("")
        if (Red)
            mesh->SetMaterial(Material( FVector(255,0,0), 0), 0);
        else
            mesh->SetMaterial(Material(FVector(0, 0, 255), 0), 0);

        SetRootComponent(mesh);

        Red = !Red;

    }

    ~QTest_ActorMinusOne()
    {
        mesh->Destroy();
    }

};
*/


#include <EngineStuff/Window/EngineWindow.h>
#include <Window/Viewport.h>
#include <UI/UIImage.h>
class QTest_ActorMinusOne : public QActor
{
    QFAClassIn(QTest_ActorMinusOne);
    static bool Red;
private:
    QFAUIImage* UIImage;
    QFAImage* Image1;
    QFAImage* Image2;

    const float Time = 5.0f;
    float SumTime = 0.0f;
    bool SetImage1 = true;
    QCameraComponent Camera = QCameraComponent(45.0f, 1000);
    QStaticMesh* mesh = nullptr;
    QFAUITextInput* Input;
public:
    QTest_ActorMinusOne()
    {        
        QFAViewport* viewport = QFAWindow::GetWindow()->GetViewport(0);
        viewport->ChangeCamera(&Camera);
        viewport->ActivateCamera();
        Camera.SetLocalPosition(FVector(-200, 0, 0));

        UIImage = new QFAUIImage(Image1 = QFAContentManager::GetImage(U"Content\\Jopa.qfa"));
        //viewport->AddUnit(UIImage);
         
        Input = new QFAUITextInput;
        Input->SetValue(U"some input text");
        viewport->AddUnit(Input);
        Input->SetTop("0");
        Input->SetLeft("0");
        Input->SetBackgroundColor(QFAColor(255,0,0));

        SetTick(false);
        Camera.SetRotation(0);

        
        QStaticMesh* mesh = NewObject<QStaticMesh>();
        mesh->SetMesh(QFAContentManager::GetMesh(U"Content\\Arrow.qfa"));
        //mesh = new QStaticMesh(QFAContentManager::GetMesh(U"Content\\untitled.qfa"));
        if (!mesh)
            stopExecute("");

        if (Red)
            mesh->SetMaterial(Material(FVector(255, 0, 0), 0), 0);
        else
            mesh->SetMaterial(Material(FVector(0, 0, 255), 0), 0);

        SetRootComponent(mesh);
        mesh->AttachComponent(&Camera);
        SetActorPosition(0);

        Red = !Red;
    }

    void Tick(float tick)
    {
        std::cout << "Game module actor tick time = " << tick << "\n";
    }

    ~QTest_ActorMinusOne()
    {
        delete UIImage;
        mesh->Destroy();
        std::cout << "~QTest_ActorMinusOne "<< "\n";
    }



};


class QTest_Actor : public QActor
{
    QFAClassIn(QTest_Actor)
private:
    
public:
    QTest_Actor()
    {
        
    }

    ~QTest_Actor()
    {
        
    }

};


