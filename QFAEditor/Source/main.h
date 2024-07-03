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


class QTest_ActorMinusOne : public QActor
{
    QFAClassIn(QTest_ActorMinusOne)
private:
    
public:
    QTest_ActorMinusOne()
    {
        std::cout << "Get mesh " << QFAContentManager::GetMesh(U"Content\Arrow.qfa") << "\n";
    }

    ~QTest_ActorMinusOne()
    {
        
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
