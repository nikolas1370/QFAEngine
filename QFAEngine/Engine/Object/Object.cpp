#include "pch.h"
#include "Object.h"
#include <Overlord/GameCode.h>

QFAClass* QObject::_QFAClassInfo;


QObject::QObject()
{
    
}

QObject::~QObject()
{
    Valid = 0;
}

QFAClass* QObject::GetClass()
{
    return QObject::_QFAClassInfo;
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


QObject* NewObject(int classId)
{
    if (QFAGameCodeFunctions* funs = QFAEngineGameCode::GetAPI())
        return funs->CreateObject(classId);
    else
        return nullptr;
}
