#pragma once
#include <vector>
#include <Tools/Stuff.h>
#include <iostream>
class QObject;
class QActor;
class QActorComponent;
// use inside QObject class
#define QFAClassIn(className) static QFAClassInfo<className> _QFAClassInfo;

// use in .cpp 
#define QFAClassOut(className) QFAClassInfo<className> className::_QFAClassInfo;

extern "C" __declspec(dllexport) void* ___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___();

class GameCodeCompiler;
/*
    only this class and child class can change some inside
*/
class QFAClass
{
    friend void* ___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___();
    
public:
    enum EBaseOn
    {
        Object,
        Actor,
        ActorComponent
    };

protected:
    const char* ClassName;
    const char* ClassRawName;

    static std::vector<QFAClass*> QCI;
    static int QfaClassCount;

    /*
        0 is reserved first id 1
        this id temporary, can change between compilations
    */
    size_t ClassId;
    EBaseOn BaseOn;

#if QFA_EDITOR_ONLY
    std::vector<QObject*> ObjectList;

#endif

    virtual QObject* CreateObjectInside() = 0;
    static QObject* CreateObject(size_t classId);    
    static void DeleteObject(QObject* object);
    static std::vector<QFAClass*>& GetGameClassList();

#if QFA_EDITOR_ONLY
    static std::vector<QObject*>& GetListObject(size_t classId);
#endif

public:
    static size_t GetGameClassCount();
    static QFAClass* GetClass(size_t classId);
    static QFAClass* GetClass(QObject* object);
    static QFAClass* GetClassByIndex(size_t index);

protected:
#if QFA_EDITOR_ONLY
    void SetCompileIndex(QObject* object, size_t index);
    size_t GetCompileIndex(QObject* object);
    void PushNewClass();

#endif

public:

    const char* GetName()
    {
        return ClassName;
    }

    const char* GetRawName()
    {
        return ClassRawName;
    }

    size_t GetId()
    {
        return ClassId;
    }    

    EBaseOn GetBaseOn()
    {
        return BaseOn;
    }    
};


template<typename T>
class QFAClassInfo : public QFAClass
{
private:
#if QFA_EDITOR_ONLY
    // Editor use this vector for HotReload ( fast delete QObject )
    QObject* CreateObjectInside() override
    {
        QObject* object = new T();
        SetCompileIndex(object, ObjectList.size());
        ObjectList.push_back(object);
        return object;
    }

#else

    QObject* QFAClassInfo<T>::CreateObjectInside()
    {
        return new T();
    }
#endif

public:
    QFAClassInfo()
    {
        static_assert(std::is_base_of<QObject, T>::value, "class " __FUNCTION__" must inherited from base QObject");        
        if (std::is_base_of<QActor, T>::value)
            BaseOn = EBaseOn::Actor;
        else if (std::is_base_of<QActorComponent, T>::value)
            BaseOn = EBaseOn::ActorComponent;
        else
            BaseOn = EBaseOn::Object;

        ClassName = typeid(T).name();
        ClassRawName = typeid(T).raw_name();
        PushNewClass();
        ClassId = ++QfaClassCount;
    }
};

/*  need in Editor, not use in game    */    
struct QFAEXPORT QFAGameCodeFunctions
{
    QObject* (*CreateObject)(size_t classId);
    void (*DeleteObject)(QObject* object);
    /*
        return list of all Game class based on QObject
    */
    std::vector<QFAClass*>& (*GetGameClassList)();

#if QFA_EDITOR_ONLY
    /*
        return list of all created object based on QObject ( Engine classes not includet )
    */
    std::vector<QObject*>& (*GetGameObjectList)(size_t ClassId);
#endif
};