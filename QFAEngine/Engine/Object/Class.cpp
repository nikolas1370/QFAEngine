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
    free(object);
    qClass->ObjectList[index] = qClass->ObjectList.back();
    qClass->SetCompileIndex(qClass->ObjectList[index], index);
    qClass->ObjectList.pop_back();

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
#if QFA_EDITOR_ONLY
    for (size_t i = ObjectClasses::MAX; i < QCI.size(); i++)
        QCIOnlyGameClass.push_back(QCI[i]);

#endif    

    
#if In_Game_Module
    for (size_t i = 0; i < ObjectClasses::MAX; i++)
        QCI[i] = engineClasses[i];

#else
    QCI[ObjectClasses::Object] = new QFAClassInfo<QObject>(true);
    QCI[ObjectClasses::Object]->ClassId = ObjectClasses::Object;
    QCI[ObjectClasses::Object]->BaseOn = ObjectClasses::Undefined;
    QCI[ObjectClasses::Object]->EngineClass = ObjectClasses::Object;
    QObject::_QFAClassInfo = QCI[ObjectClasses::Object];

    QCI[ObjectClasses::Actor] = new QFAClassInfo<QActor>(true);
    QCI[ObjectClasses::Actor]->ClassId = ObjectClasses::Actor;
    QCI[ObjectClasses::Actor]->BaseOn = ObjectClasses::Object;
    QCI[ObjectClasses::Actor]->EngineClass = ObjectClasses::Actor;
    QActor::_QFAClassInfo = QCI[ObjectClasses::Actor];

    QCI[ObjectClasses::ActorComponent] = new QFAClassInfo<QActorComponent>(true);
    QCI[ObjectClasses::ActorComponent]->ClassId = ObjectClasses::ActorComponent;
    QCI[ObjectClasses::ActorComponent]->BaseOn = ObjectClasses::Object;
    QCI[ObjectClasses::ActorComponent]->EngineClass = ObjectClasses::ActorComponent;
    QActorComponent::_QFAClassInfo = QCI[ObjectClasses::ActorComponent];

    QCI[ObjectClasses::SceneComponent] = new QFAClassInfo<QSceneComponent>(true);
    QCI[ObjectClasses::SceneComponent]->ClassId = ObjectClasses::SceneComponent;
    QCI[ObjectClasses::SceneComponent]->BaseOn = ObjectClasses::ActorComponent;
    QCI[ObjectClasses::SceneComponent]->EngineClass = ObjectClasses::SceneComponent;
    QSceneComponent::_QFAClassInfo = QCI[ObjectClasses::SceneComponent];

    QCI[ObjectClasses::StaticMeshActor] = new QFAClassInfo<AStaticMeshActor>(true);
    QCI[ObjectClasses::StaticMeshActor]->ClassId = ObjectClasses::StaticMeshActor;
    QCI[ObjectClasses::StaticMeshActor]->BaseOn = ObjectClasses::Actor;
    QCI[ObjectClasses::StaticMeshActor]->EngineClass = ObjectClasses::StaticMeshActor;
    AStaticMeshActor::_QFAClassInfo = QCI[ObjectClasses::StaticMeshActor];

    QCI[ObjectClasses::CameraActor] = new QFAClassInfo<ACameraActor>(true);
    QCI[ObjectClasses::CameraActor]->ClassId = ObjectClasses::CameraActor;
    QCI[ObjectClasses::CameraActor]->BaseOn = ObjectClasses::Actor;
    QCI[ObjectClasses::CameraActor]->EngineClass = ObjectClasses::CameraActor;
    ACameraActor::_QFAClassInfo = QCI[ObjectClasses::CameraActor];

    QCI[ObjectClasses::CameraComponent] = new QFAClassInfo<QCameraComponent>(true);
    QCI[ObjectClasses::CameraComponent]->ClassId = ObjectClasses::CameraComponent;
    QCI[ObjectClasses::CameraComponent]->BaseOn = ObjectClasses::SceneComponent;
    QCI[ObjectClasses::CameraComponent]->EngineClass = ObjectClasses::CameraComponent;
    QCameraComponent::_QFAClassInfo = QCI[ObjectClasses::CameraComponent];

    QCI[ObjectClasses::MeshBase] = new QFAClassInfo<QMeshBaseComponent>(true);
    QCI[ObjectClasses::MeshBase]->ClassId = ObjectClasses::MeshBase;
    QCI[ObjectClasses::MeshBase]->BaseOn = ObjectClasses::SceneComponent;
    QCI[ObjectClasses::MeshBase]->EngineClass = ObjectClasses::MeshBase;
    QMeshBaseComponent::_QFAClassInfo = QCI[ObjectClasses::MeshBase];

    QCI[ObjectClasses::StaticMesh] = new QFAClassInfo<QStaticMesh>(true);
    QCI[ObjectClasses::StaticMesh]->ClassId = ObjectClasses::StaticMesh;
    QCI[ObjectClasses::StaticMesh]->BaseOn = ObjectClasses::MeshBase;
    QCI[ObjectClasses::StaticMesh]->EngineClass = ObjectClasses::StaticMesh;
    QStaticMesh::_QFAClassInfo = QCI[ObjectClasses::StaticMesh];

    QCI[ObjectClasses::World] = new QFAClassInfo<QWorld>(true);
    QCI[ObjectClasses::World]->ClassId = ObjectClasses::World;
    QCI[ObjectClasses::World]->BaseOn = ObjectClasses::Object;
    QCI[ObjectClasses::World]->EngineClass = ObjectClasses::World;
    QWorld::_QFAClassInfo = QCI[ObjectClasses::World];

    QCI[ObjectClasses::DirectionLight] = new QFAClassInfo<QDirectionLight>(true);
    QCI[ObjectClasses::DirectionLight]->ClassId = ObjectClasses::DirectionLight;
    QCI[ObjectClasses::DirectionLight]->BaseOn = ObjectClasses::Object;
    QCI[ObjectClasses::DirectionLight]->EngineClass = ObjectClasses::DirectionLight;
    QDirectionLight::_QFAClassInfo = QCI[ObjectClasses::DirectionLight];
#endif
}

