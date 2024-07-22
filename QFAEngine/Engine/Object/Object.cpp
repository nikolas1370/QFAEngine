#include "pch.h"
#include "Object.h"


QFAClass::ObjectClasses QObject::GetEngineClassId()
{
    return QFAClass::ObjectClasses::Object;
}

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

    this->~QObject();
    free(this);
}
