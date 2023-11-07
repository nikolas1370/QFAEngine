#include "Object.h"

QObject::QObject()
{

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
