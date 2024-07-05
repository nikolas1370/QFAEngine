#include "Class.h"
#include <iostream>
#include <Object/Object.h>

int QFAClass::QfaClassCount = 0;
std::vector<QFAClass*> QFAClass::QCI;

QFAGameCodeFunctions QFCFs;

extern "C" void* ___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___()
{
    QFCFs.CreateObject = &QFAClass::CreateObject;
    QFCFs.DeleteObject = &QFAClass::DeleteObject;
    QFCFs.GetGameClassList = &QFAClass::GetGameClassList;
#if QFA_EDITOR_ONLY
    QFCFs.GetGameObjectList = &QFAClass::GetListObject;
#endif
    return &QFCFs;
}

void QFAClass::PushNewClass()
{
    QCI.push_back(this);
}

QObject* QFAClass::CreateObject(size_t classId)
{
    if (QCI.size() < classId)
    {
        std::cout << "QFAClass::CreateObject nulptr\n";
        return nullptr;
    }

    QObject* obj = QCI[classId - 1]->CreateObjectInside();
    obj->Class = QCI[classId - 1];
    return obj;
}

void QFAClass::DeleteObject(QObject* object)
{    
#if QFA_EDITOR_ONLY

    if (!object->IsValid())
        return;

    QFAClass* qClass = object->Class;
    if (qClass->ObjectList.size() <= qClass->GetCompileIndex(object) ||
        qClass->ObjectList[qClass->GetCompileIndex(object)] != object)
    {
        std::cout << "QFAClassInfo::DeleteObject\n";
        return;
    }

    size_t index = qClass->GetCompileIndex(object);
    delete object;
    qClass->ObjectList[index] = qClass->ObjectList.back();
    qClass->SetCompileIndex(qClass->ObjectList[index], index);
    qClass->ObjectList.pop_back();

#else    

    delete object;

#endif
}

std::vector<QFAClass*>& QFAClass::GetGameClassList()
{
    return QCI;
}

QFAClass* QFAClass::GetClass(size_t classId)
{
    if (QCI.size() < classId)
    {
        std::cout << "QFAClass::GetClass nulptr\n";
        return nullptr;
    }

    return QCI[classId - 1];
}

QFAClass* QFAClass::GetClass(QObject* object)
{
    if (!object->IsValid())
        return nullptr;

    return object->Class;
}

QFAClass* QFAClass::GetClassByIndex(size_t index)
{
    if (QCI.size() < index + 1)
    {
        std::cout << "QFAClass::GetClassByIndex nulptr\n";
        return nullptr;
    }
    
    return QCI[index];
}

size_t QFAClass::GetGameClassCount()
{
    return QCI.size();
}



#if QFA_EDITOR_ONLY

std::vector<QObject*>& QFAClass::GetListObject(size_t classId)
    {
        return QCI[classId - 1]->ObjectList;
    }

    void QFAClass::SetCompileIndex(QObject* object, size_t index)
    {
        object->CompileIndex = index;
    }
    
    size_t QFAClass::GetCompileIndex(QObject* object)
    {
        return object->CompileIndex;
    }

#endif 