#pragma once
#include <iostream>
#include "../../QFAEngine/Engine/Object/Object.h"
#include "../../QFAEngine/Engine/Object/Actor/Actor.h"
#include <Overlord/ContentManager.h>
#include <UI/TextInput.h>

#include "SomeCode.h"

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

#include <Object/ActorComponent/SceneComponent/AudioSceneComponent.h>

class QTest_ActorMinusOne : public QActor
{
    QFAClassIn(QTest_ActorMinusOne);

    QAudioSceneComponent audio; 

    const float Time = 5.0f;
    float SumTime = 0.0f;
    bool SetImage1 = true;
    QCameraComponent Camera = QCameraComponent(45.0f, 1000);
    QStaticMesh* mesh = nullptr;
public:
    QTest_ActorMinusOne()
    {        

        Camera.SetLocalPosition(FVector(-200, 0, 0));


        SetTick(true);
        Camera.SetRotation(0);

        
        QStaticMesh* mesh = NewObject<QStaticMesh>();
        mesh->SetMesh(QFAContentManager::GetMesh(U"Content\\Arrow.qfa"));
        if (!mesh)
            stopExecute("");

        SetRootComponent(mesh);
        mesh->AttachComponent(&Camera);
        SetActorPosition(0);
        audio.SetAudio(U"videoplayback_2.mp3", true);
        audio.SetRepeat(true);

        
        std::cout << GetClass()->GetName() << "\n";
        mesh->AttachComponent(&audio);
        Camera.ActivateAudio(true);
        audio.Play(); 

        
    }

    void Tick(float tick)
    {

    }

    ~QTest_ActorMinusOne()
    {
        mesh->Destroy();
        std::cout << "~QTest_ActorMinusOne "<< "\n";
    }



};
/*

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
*/

