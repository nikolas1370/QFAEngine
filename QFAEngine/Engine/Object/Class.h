#pragma once
#include <vector>
#include <Tools/Stuff.h>
#include <iostream>
#include <EngineStuff/EngineClass.h>
#include <Object/Object.h>

class QFAClass;
template<typename T>
class QFAClassInfo;
class QFAOverlord;
class QFAGameCode;
class QFAUIEditorFileExplorer;
class QFAEditorMainWindow;
class QFAClassInstance : public QFAEngineClassInstance
{
    friend QFAClass;
    friend QFAClassInfo;
    friend QFAOverlord;
    friend QFAGameCode;
    friend QFAUIEditorFileExplorer;
    friend QFAEditorMainWindow;
    inline static QFAClassInstance* Class = nullptr;

    // first QFAObjectClasses::MAX classes is engine class
    std::vector<QFAClass*> QCI;    

    static inline bool ClassInit = false;
    // call after load game module
    void InitClasses(std::vector<QFAClass*> engineClasses) override
    {
        if (QFAClassInstance::ClassInit)
            stopExecute("not call InitClasses twice");

        QFAClassInstance::ClassInit = true;
        for (size_t i = 0; i < engineClasses.size(); i++)
            QCI[i] = engineClasses[i];

        BuildClassesTree(QCI);        
        for (size_t i = QFAObjectClasses::QOCMAX; i < QCI.size(); i++)
            GameQCI.push_back(QCI[i]);
    }

    void FreeAll()
    {
        if (!QCI.size())
            return;
 
        QCI.clear(); // don't delete QCI
    }

protected:
    // create same function for game code but with copy vector
    std::vector<QFAClass*>& GetClassList() override
    {
        return QCI;
    }

public:
    static QFAClassInstance* GetClassInstance()
    {
#if In_Game_Module
        if (Class)
            return Class;
        else
            return Class = new QFAClassInstance;
#else
        return (QFAClassInstance*)QFAEngineClassInstance::GetGameClassInstance();
#endif 
    }

    QObject* CreateObject(size_t classId) override
    {
        if (QCI.size() <= classId)
            return nullptr;

        return CreateObjectInside(QCI[classId]);
    }

    inline QObject* CreateObjectByName(const char* className) override
    {
        return CreateObjectByNameInside(className, QCI);
    }

    QFAClass* GetClass(size_t id)
    {
        if (QCI.size() <= id)
            return nullptr;

        return QCI[id];
    }
};

class QObject;
class QActor;
class QActorComponent;
class QSceneComponent;
// use inside QObject class  and put in start class before all other stuff
#define QFACLASS(className)                                                \
    inline static QFAClassInfo<className> _QFAClassInfo;                   \
    static QFAClass* GetClassStatic() {return &className::_QFAClassInfo;}  \
    static QFAClass* GetParentClass() {return __super::GetClassStatic();}  \
public:                                                                    \
    virtual QFAClass* GetClass() {return &className::_QFAClassInfo;}       \
    template<typename T>                                                   \
    friend QObject* NewObject();                                           \
    friend QFAClassInfo;                                                   \
private:

class QFAClass;
class QFAClassInstance;
class GameCodeCompiler;
class QFAGameCode;
class QFAOverlord;
class QFAClass
{
    friend QFAOverlord;
    friend QFAGameCode;
    friend QFAClassInstance;
    friend QFAEngineClassInstance;

public:
    QObject* TemObject = nullptr;

protected:
    const char* ClassName;
    const char* ClassRawName;
    bool IsAbstract;
    QFAClass* ParentClass = nullptr;
    std::vector<QFAClass*> ChildrenList;

    /*
        this id temporary, can change between compilations
    */
    QFAObjectClasses ClassId = QFAObjectClasses::QOCUndefined;
    // can be only Object, Actor, ActorComponent and SceneComponent (Undefined for QObject)
    QFAObjectClasses BaseOn = QFAObjectClasses::QOCUndefined;
    // if Undefined this not engine class
    QFAObjectClasses EngineClass = QFAObjectClasses::QOCUndefined;

protected:
    virtual void SetParent() = 0;
    virtual QObject* CreateObjectInside() { return nullptr; };

    static inline void AddChild(QFAClass* parent, QFAClass* child)
    {
        parent->ChildrenList.push_back(child);
    }

    QFAClass()
    {
#if In_Game_Module
        QFAClassInstance* instance = QFAClassInstance::GetClassInstance();
        if (!instance->QCI.size()) 
            instance->QCI.resize(QFAObjectClasses::QOCMAX);
#endif
    }

public:
    /*
        game class start from QFAObjectClasses::MAX index
        do not delete QFAClass
    */
    static QFAClass* GetClass(int classId)
    {
        QFAClassInstance* instance = QFAClassInstance::GetClassInstance();
        if (instance->QCI.size() <= classId || classId < 0)
            return nullptr;

        return instance->QCI[classId];
    }
        
    // do not delete QFAClass
    static QFAClass* GetClass(QObject* object)
    {
        if (!object->IsValid())
            return nullptr;

        return object->GetClass();
    }

public:

    inline const char* GetName()
    {
        return ClassName;
    }

    inline const char* GetRawName()
    {
        return ClassRawName;
    }

    inline size_t GetId()
    {
        return ClassId;
    }    

    inline QFAObjectClasses GetBaseOn()
    {
        return BaseOn;
    }    

    inline bool GetIsAbstract()
    {
        return IsAbstract;
    }

    inline QFAClass* GetParentClass()
    {
        return ParentClass;
    }    

    inline QFAClass* GetChildClass(size_t index)
    {
        return ChildrenList[index];
    }

    inline size_t GetChildClassAmount()
    {
        return ChildrenList.size();
    }

    void GetChildrenClasses(std::vector<QFAClass*>& out)
    {
        for (size_t i = 0; i < ChildrenList.size(); i++)
            out.push_back(ChildrenList[i]);
    }

    static size_t GetGameClassCount()
    {
        QFAClassInstance* instance = QFAClassInstance::GetClassInstance();
        return instance->QCI.size() - QFAObjectClasses::QOCMAX;
    }

    static size_t GetEngineClassCount()
    {
        return QFAObjectClasses::QOCMAX;
    }

    static size_t GetClassCount()
    {
        QFAClassInstance* instance = QFAClassInstance::GetClassInstance();
        return instance->QCI.size();
    }


};

template<typename T>
class QFAClassInfo : public QFAClass
{
    friend QFAEngineClassInstance;
    friend QObject;

#if QFA_EDITOR_ONLY
    QObject* CreateObjectInside() override
    {
        return COI<T>();
    }

    // Editor use this vector for HotReload ( fast delete QObject )
    template<typename T> 
    requires std::is_abstract<T>::value// requires canot be virtual
    QObject* COI()  // if class abstract object not created
    {  
        stopExecute("abstract can't be created");
        return nullptr;
    }

    template<typename T>
        requires !std::is_abstract<T>::value
    QObject* COI() 
    {
        QObject* object = (T*)malloc(sizeof(T));
        
        new (object) T();
#if QFA_EDITOR_ONLY

        object->CreateInApi = true;
#endif

        return object;
    }

#else

    QObject* QFAClassInfo<T>::CreateObjectInside()
    {
        QObject* object = (T*)malloc(sizeof(T));        
        return new (object) T();
    }
#endif
    // use only for engine
    QFAClassInfo(bool insideUsage) 
    {
        static_assert(std::is_base_of<QObject, T>::value, "class " __FUNCTION__" must inherited from base QObject");
        if (std::is_base_of<QActor, T>::value)
            BaseOn = QFAObjectClasses::QOCActor;
        else if (std::is_base_of<QSceneComponent, T>::value)
            BaseOn = QFAObjectClasses::QOCSceneComponent;
        else if (std::is_base_of<QActorComponent, T>::value)
            BaseOn = QFAObjectClasses::QOCActorComponent;
        else
            BaseOn = QFAObjectClasses::QOCObject;

        ClassName = typeid(T).name();
        ClassRawName = typeid(T).raw_name();
        IsAbstract = std::is_abstract<T>::value;
    }

    void SetParent() override
    {
        ParentClass = T::GetParentClass();
        if (ParentClass)
            QFAClass::AddChild(ParentClass, this);
    }

public:
    QFAClassInfo()
    {
        static_assert(std::is_base_of<QObject, T>::value, "class " __FUNCTION__" must inherited from base QObject");        
        if (std::is_base_of<QActor, T>::value)
            BaseOn = QFAObjectClasses::QOCActor;
        else if (std::is_base_of<QActorComponent, T>::value)
            BaseOn = QFAObjectClasses::QOCActorComponent;
        else if (std::is_base_of<QSceneComponent, T>::value)
            BaseOn = QFAObjectClasses::QOCSceneComponent;
        else
            BaseOn = QFAObjectClasses::QOCObject;
        
        QFAClassInstance* instance = QFAClassInstance::GetClassInstance();
        ClassName = typeid(T).name();
        ClassRawName = typeid(T).raw_name();
        IsAbstract = std::is_abstract<T>::value;
        ClassId = (QFAObjectClasses)instance->QCI.size();
        instance->QCI.push_back(this);
    }
};

