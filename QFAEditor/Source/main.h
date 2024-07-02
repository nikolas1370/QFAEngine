#pragma once
#include <iostream>
#include "../../QFAEngine/Engine/Object/Object.h"
#include "../../QFAEngine/Engine/Object/Actor/Actor.h"


int x = 15;

class Test : public QObject
{
    QFAClassIn(Test)    
private:
    
public:
    Test()
    {
        std::cout << "QTest constructor " <<x <<"\n";
    }

    ~Test()
    {
        std::cout << "~QTest detructor " <<x <<"\n";
    }
};


class QTest_ActorMinusOne //: public QActor
{
    //QFAClassIn(QTest_ActorMinusOne)
private:
    
public:
    QTest_ActorMinusOne()
    {
        std::cout << "QTest_ActorMinusOne constructor " <<x <<"\n";
    }

    ~QTest_ActorMinusOne()
    {
        std::cout << "~QTest_ActorMinusOne detructor " << x<<"\n";
    }

};

class QTest_Actor : public QActor
{
    QFAClassIn(QTest_Actor)
private:
    
public:
    QTest_Actor()
    {
        std::cout << "QTest_Actor constructor " <<x <<"\n";
    }

    ~QTest_Actor()
    {
        std::cout << "~QTest_Actor detructor " << x<<"\n";
    }

};
