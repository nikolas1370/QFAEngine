#pragma once
#include <iostream>
#include "../../QFAEngine/Engine/Object/Object.h"





class QTest : public QObject
{
    QFAClassIn(QTest)
private:
    
public:
    QTest()
    {
        std::cout << "QTest constructor\n";
    }

    ~QTest()
    {
        std::cout << "~QTest detructor\n";
    }

};


class QTest_two : public QObject
{
    QFAClassIn(QTest_two)
private:
    
public:
    QTest_two()
    {
        std::cout << "QTest_two constructor\n";
    }

    ~QTest_two()
    {
        std::cout << "~QTest_two detructor\n";
    }

};

class QTest_tru : public QObject
{
    QFAClassIn(QTest_tru)
private:
    
public:
    QTest_tru()
    {
        std::cout << "QTest_tru constructor\n";
    }

    ~QTest_tru()
    {
        std::cout << "~QTest_tru detructor\n";
    }

};