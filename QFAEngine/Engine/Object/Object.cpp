#include "Object.h"



// remove
//#include <iostream>

QObject::QObject()
{
    /*
    // __func__

       #include <typeinfo>
    // typeid(this).name()
    */
    
    //std::cout << typeid(this).name() << std::endl;
}

QObject::~QObject()
{
    Valid = 0;
}

bool QObject::IsValid()
{
    return this && Valid == ValidNumber;
}

void QObject::Destroy()
{
    if (!IsValid())
        return;
    
    delete this;
}
