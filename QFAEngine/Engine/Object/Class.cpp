#include "Class.h"
#include <iostream>
#include <Object/Object.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/ActorComponent.h>
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
#include <Object/Actor/StaticMeshActor.h>
#include <Object/Actor/Camera/CameraActor.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Object/World/World.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Object/Actor/Camera/FlyingCamera.h>

bool QFAClass::ClassInit = false;
std::vector<QFAClass*> QFAClass::QCI;
#if QFA_EDITOR_ONLY
std::vector<QFAClass*> QFAClass::QCIOnlyGameClass;
#endif
QFAGameCodeFunctions QFCFs;

QObject* QFAClass::CreateObject(size_t classId)
{
#if In_Game_Module
    
#else
    stopExecute("QFAClass::CreateObject can be call only in GameModule");
#endif

    if (QCI.size() < classId)
        return nullptr;

    return QCI[classId]->CreateObjectInside();
}

QObject* QFAClass::CreateObjectByName(const char* className)
{    
#if In_Game_Module
    
#else
    stopExecute("QFAClass::CreateObjectByName can be call only in GameModule");
#endif


    for (size_t i = 0; i < QCI.size(); i++)
        if (!strcmp(className, QCI[i]->ClassName)) 
            return QCI[QCI[i]->ClassId]->CreateObjectInside();
    
    return nullptr;
}

void QFAClass::DeleteObject(QObject* object)
{    

#if In_Game_Module
    
#else
    stopExecute("QFAClass::CreateObjectByName can be call only in GameModule");
#endif

#if QFA_EDITOR_ONLY

    if (!object->IsValid())
        return;

    QFAClass* qClass = object->GetClass();

    if (qClass->ObjectList.size() <= qClass->GetCompileIndex(object) ||
        qClass->ObjectList[qClass->GetCompileIndex(object)] != object)
    {
        stopExecute("not found object in list");
        return;
    }

    size_t index = qClass->GetCompileIndex(object);
    object->~QObject();
    qClass->ObjectList[index] = qClass->ObjectList.back();
    qClass->SetCompileIndex(qClass->ObjectList[index], index);
    qClass->ObjectList.pop_back();

    free(object);

#else    

    object~>~QObject();
    free(object);

#endif
}

std::vector<QFAClass*>& QFAClass::GetGameClassList()
{
    return QCIOnlyGameClass;
}

QFAEXPORT QFAClass** QFAClass::GetEngineClassList()
{
    return QCI.data();
}

std::vector<QFAClass*>& QFAClass::GetClassList()
{
    return QCI;
}

QFAClass* QFAClass::GetClass(int classId)
{
    if (QCI.size() <= classId || classId < 0)
        return nullptr;

    return QCI[classId];
}

QFAClass* QFAClass::GetClass(QObject* object)
{
    if (!object->IsValid())
        return nullptr;

    return object->GetClass();
}

size_t QFAClass::GetGameClassCount()
{
    return QCI.size() - ObjectClasses::MAX;
}

size_t QFAClass::GetEngineClassCount()
{
    return ObjectClasses::MAX;
}

size_t QFAClass::GetClassCount()
{
    return QCI.size();
}

#if QFA_EDITOR_ONLY
    std::vector<QObject*>& QFAClass::GetListObject(size_t classId)
    {
        return QCI[classId]->ObjectList;
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

extern "C" void* ___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___(QFAClass** engineClasses)
{
    // inside call QFATextLocalization::SetTextList
    QFAClass::InitClasses(engineClasses);    
    QFCFs.CreateObject = &QFAClass::CreateObject;
    QFCFs.CreateObjectByName = &QFAClass::CreateObjectByName;
    QFCFs.DeleteObject = &QFAClass::DeleteObject;
    QFCFs.GetGameClassList = &QFAClass::GetGameClassList;
    QFCFs.GetClassList = &QFAClass::GetClassList;
#if QFA_EDITOR_ONLY
    QFCFs.GetGameObjectList = &QFAClass::GetListObject;
#endif
   return &QFCFs;
}

void ___QFAGAMECODEEXPORTFUNCTIONFreeClasses___()
{
    if (!QFAClass::QCI.size())
        return;
        
#if QFA_EDITOR_ONLY
    QFAClass::QCIOnlyGameClass.clear();    

#endif    
    QFAClass::QCI.clear();
}


#define SetEngineClass(engineClassName, engineClassId)              \
    QCI[engineClassId] = new QFAClassInfo<engineClassName>(true);   \
    QCI[engineClassId]->ClassId = engineClassId;                    \
    QCI[engineClassId]->EngineClass = engineClassId;                \
    engineClassName::_QFAClassInfo = QCI[engineClassId];

    
void QFAClass::InitClasses(QFAClass** engineClasses)
{
    if (QFAClass::ClassInit)
        stopExecute("not call InitClasses twice");

    QFAClass::ClassInit = true;
    /*
        if add new class in engine need add
        QFAEngineClassIn()
        QFAEngineClassOut()
    */
#if QFA_EDITOR_ONLY && In_Game_Module
    for (size_t i = ObjectClasses::MAX; i < QCI.size(); i++)
        QCIOnlyGameClass.push_back(QCI[i]);

#endif    

#if In_Game_Module
    for (size_t i = 0; i < ObjectClasses::MAX; i++)
        QCI[i] = engineClasses[i];

    // build class parent child tree
    for (size_t i = 1; i < QCI.size(); i++)
        QCI[i]->SetParent();    

#else



    SetEngineClass(QObject,              ObjectClasses::Object);
    SetEngineClass(QActor,               ObjectClasses::Actor);
    SetEngineClass(QActorComponent,      ObjectClasses::ActorComponent);
    SetEngineClass(QSceneComponent,      ObjectClasses::SceneComponent);
    SetEngineClass(AStaticMeshActor,     ObjectClasses::StaticMeshActor);
    SetEngineClass(ACameraActor,         ObjectClasses::CameraActor);
    SetEngineClass(QCameraComponent,     ObjectClasses::CameraComponent);
    SetEngineClass(QMeshBaseComponent,   ObjectClasses::MeshBase);
    SetEngineClass(QStaticMesh,          ObjectClasses::StaticMesh);
    SetEngineClass(QWorld,               ObjectClasses::World);
    SetEngineClass(QDirectionLight,      ObjectClasses::DirectionLight);
    SetEngineClass(QAudioSceneComponent, ObjectClasses::AudioSceneComponent);
    SetEngineClass(AFlyingCamera,        ObjectClasses::FlyingCamera);

#endif

}

