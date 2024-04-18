#pragma once
#include "../../QFAEngine/Engine/Object/Object.h"

class SomeCode : public QObject
{
    QFAClassIn(SomeCode)
private:
    /* data */
public:
    SomeCode(/* args */);
    ~SomeCode();
};


