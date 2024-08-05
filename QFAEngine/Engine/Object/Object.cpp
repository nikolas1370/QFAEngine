#include "pch.h"
#include "Object.h"
#include <Overlord/GameCode.h>

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
#if QFA_EDITOR_ONLY
    if (CreateInApi)
        QFAEngineGameCode::GetAPI()->DeleteObject(this);
    else
    {
        this->~QObject();
        free(this);
    } 
#else
    this->~QObject();
    free(this);
#endif
}
