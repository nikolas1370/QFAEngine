#pragma once
#include "../../QFAEngine/Engine/Object/Object.h"

class SomeCode : public QObject
{
    QFAClassIn(SomeCode)
private:
    
public:
    SomeCode();
    ~SomeCode();
    int  l;
}; 


