#pragma once
#include <iostream>
#include "../../QFAEngine/Engine/Object/Object.h"



int x = 2;

class QTest : public QObject
{
    QFAClassIn(QTest)
private:
    
public:
    QTest()
    {
        std::cout << "QTest constructor " <<x <<"\n";
    }

    ~QTest()
    {
        std::cout << "~QTest detructor " <<x <<"\n";
    }

};


class QTest_two : public QObject
{
    QFAClassIn(QTest_two)
private:
    
public:
    QTest_two()
    {
        std::cout << "QTest_two constructor " <<x <<"\n";
    }

    ~QTest_two()
    {
        std::cout << "~QTest_two detructor " << x<<"\n";
    }

};

class QTest_tru : public QObject
{
    QFAClassIn(QTest_tru)
private:
    
public:
    QTest_tru()
    {
        std::cout << "QTest_tru constructor " <<x <<"\n";
    }

    ~QTest_tru()
    {
        std::cout << "~QTest_tru detructor " << x<<"\n";
    }

};