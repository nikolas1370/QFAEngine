#include "pch.h"
#include "Object.h"
#include <Object/Class.h> // don't move in .h

QFAClass* QObject::_QFAClassInfo;


QFAClass* QObject::GetClassStatic()
{
    return QObject::_QFAClassInfo;
}

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
        QFAEngineClassInstance::GetGameClassInstance()->DeleteObject(this);
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
    if (QFAClassInstance* instance = QFAClassInstance::GetClassInstance())
        return instance->CreateObject(classId);
    else
        return nullptr;
}

/*
#if In_Game_Module
    if (QFAClassInstance* instance = QFAClassInstance::GetClassInstance())
#else
    if (QFAEngineClassInstance* instance = QFAEngineClassInstance::GetGameClassInstance())
#endif // In_Game_Module
*/