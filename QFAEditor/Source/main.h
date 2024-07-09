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
/*
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
        /*
        
        якшо якшо запучтить редактор добать Arrow.qfa 
        і тоді цей клас заспавнить тобуде проблема
        
        

        в картінку тоже нада файл передавать і удалять його якшо він є
        * /

        mesh = new QStaticMesh(QFAContentManager::GetMesh(U"Content\\Arrow.qfa"));        
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
        std::cout << "delete\n";
        delete mesh;
    }

};
*/

#include <Render/Window/Window.h>
#include <Render/Window/Viewport.h>
#include <Render/UI/UIImage.h>
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
public:
    QTest_ActorMinusOne()
    {
        QFAViewport* viewport = QFAWindow::GetMainWindow()->GetViewport(0);

        /*
            зробить шоб неможна було це зробить в едіторі тіки коли ігра запущина

        */

        UIImage = new QFAUIImage(Image1 = QFAContentManager::GetImage(U"Content\\Jopa.qfa"));
        viewport->AddUnit(UIImage);
        Red = !Red;
    }

    ~QTest_ActorMinusOne()
    {
        std::cout << "delete\n";
        delete UIImage;
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


