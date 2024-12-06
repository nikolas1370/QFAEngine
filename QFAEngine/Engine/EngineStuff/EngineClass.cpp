#include "pch.h"
#include "EngineClass.h"
#include <Object/Class.h>
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

QFAEngineClassInstance* QFAEngineClassInstance::GameModuleClass = nullptr;
QFAClassInstance* QFAEngineClassInstance::EngineeClassInstance = nullptr;

#define SetEngineClass(engineClassName, engineClassId)                                     \
    EngineeClassInstance->EQCI[engineClassId] = new QFAClassInfo<engineClassName>(true);   \
    EngineeClassInstance->EQCI[engineClassId]->ClassId = engineClassId;                    \
    EngineeClassInstance->EQCI[engineClassId]->EngineClass = engineClassId;                \
    engineClassName::_QFAClassInfo = EngineeClassInstance->EQCI[engineClassId];

void QFAEngineClassInstance::InitClassesEngine()
{    
    if (EngineeClassInstance)
        return;

    EngineeClassInstance = new QFAClassInstance;
    EngineeClassInstance->EQCI.resize(QFAObjectClasses::QOCMAX);
    EngineeClassInstance->EQCI[1] = new QFAClassInfo<QObject>(true);
    EngineeClassInstance->EQCI[1]->ClassId = QFAObjectClasses::QOCObject;
    EngineeClassInstance->EQCI[1]->EngineClass = QFAObjectClasses::QOCObject;
    QObject::_QFAClassInfo = EngineeClassInstance->EQCI[1];

    SetEngineClass(QObject, QFAObjectClasses::QOCObject);
    SetEngineClass(QActor, QFAObjectClasses::QOCActor);
    SetEngineClass(QActorComponent, QFAObjectClasses::QOCActorComponent);
    SetEngineClass(QSceneComponent, QFAObjectClasses::QOCSceneComponent);
    SetEngineClass(AStaticMeshActor, QFAObjectClasses::QOCStaticMeshActor);
    SetEngineClass(ACameraActor, QFAObjectClasses::QOCCameraActor);
    SetEngineClass(QCameraComponent, QFAObjectClasses::QOCCameraComponent);
    SetEngineClass(QMeshBaseComponent, QFAObjectClasses::QOCMeshBase);
    SetEngineClass(QStaticMesh, QFAObjectClasses::QOCStaticMesh);
    SetEngineClass(QWorld, QFAObjectClasses::QOCWorld);
    SetEngineClass(QDirectionLight, QFAObjectClasses::QOCDirectionLight);
    SetEngineClass(QAudioSceneComponent, QFAObjectClasses::QOCAudioSceneComponent);
    SetEngineClass(AFlyingCamera, QFAObjectClasses::QOCFlyingCamera);
}

QFAEngineClassInstance::QFAEngineClassInstance()
{
}

std::vector<QFAClass*>& QFAEngineClassInstance::GetEngineClass()
{
    return EngineeClassInstance->EQCI;
}

std::vector<QFAClass*>& QFAEngineClassInstance::GetGameClassList()
{
    return GameQCI;
}

void QFAEngineClassInstance::SetGameClassInstance(QFAEngineClassInstance* localInstance)
{
	GameModuleClass = localInstance;
}

void QFAEngineClassInstance::BuildClassesTree(std::vector<QFAClass*>& classes)
{// build class parent child tree
    for (size_t i = 1; i < classes.size(); i++)
        classes[i]->SetParent();
}

void QFAEngineClassInstance::DeleteObject(QObject* object)
{
#if QFA_EDITOR_ONLY
    // need for memory management in future
    object->~QObject();
    free(object);

#else    

    object->~QObject();
    free(object);

#endif
}

QObject* QFAEngineClassInstance::CreateObjectInside(QFAClass* clas)
{
    return clas->CreateObjectInside();
}

QObject* QFAEngineClassInstance::CreateObjectByNameInside(const char* className, std::vector<QFAClass*>& classes)
{
    for (size_t i = 0; i < classes.size(); i++)
        if (!strcmp(className, classes[i]->ClassName))
            return classes[classes[i]->ClassId]->CreateObjectInside();

    return nullptr;
}

QFAEngineClassInstance* QFAEngineClassInstance::GetGameClassInstance()
{
    return GameModuleClass;
}

std::vector<QFAClass*>& QFAEngineClassInstance::GetClassList()
{
    if (GameModuleClass)
        return GameModuleClass->GetClassList();
    else
        return EQCI;
}

