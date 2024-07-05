#pragma once
#include <iostream>
#include "../../QFAEngine/Engine/Object/Object.h"
#include "../../QFAEngine/Engine/Object/Actor/Actor.h"
#include <Overlord/ContentManager.h>
;

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

#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
class QTest_ActorMinusOne : public QActor
{
    QFAClassIn(QTest_ActorMinusOne);
    static bool Red;
private:
    QStaticMesh* mesh;
public:
    QTest_ActorMinusOne()
    {
        //mesh = new QStaticMesh(QFAContentManager::GetMesh(U"Content\\Arrow.qfa"));        
        mesh = new QStaticMesh(QFAContentManager::GetMesh(U"Content\\untitled.qfa"));

        if (Red)
            mesh->SetMaterial(Material( FVector(255,0,0), 0), 0);
        else
            mesh->SetMaterial(Material(FVector(0, 0, 255), 0), 0);

        SetRootComponent(mesh);

        Red = !Red;
    }

    ~QTest_ActorMinusOne()
    {
        std::cout << "delete\n";
        delete mesh;
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


