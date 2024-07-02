#include "SomeCode.h"
#include <iostream>

QFAClassOut(SomeCode)
SomeCode::SomeCode()
{
    std::cout << "SomeCode constructor\n" ;
}

SomeCode::~SomeCode()
{
    std::cout << "~SomeCode destructor\n" ;
}
