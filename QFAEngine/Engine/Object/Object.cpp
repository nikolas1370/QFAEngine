#include "pch.h"
#include "Object.h"

QObject::QObject()
{
    
}

QObject::~QObject()
{
    Valid = 0;
}



void QObject::Destroy()
{
    if (!IsValid())
        return;
    
    delete this;
}
