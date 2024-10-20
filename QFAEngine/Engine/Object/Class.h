#pragma once
/*
    compile Class.cpp in gamecode module
    if not compile linker be mad
    engine and game module have own list of classes

    use ___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___ if wanna have access to game classes (LoadLibraryW >> GetProcAddress)
*/
#include <vector>
#include <Tools/Stuff.h>
#include <iostream>
class QObject;
class QActor;
class QActorComponent;
// use inside QObject class  and put in start class before all other stuff
#define QFAClassIn(className) static QFAClassInfo<className> _QFAClassInfo;\
                              public:\
                              virtual QFAClass* GetClass() {return &className::_QFAClassInfo;};\
                              template<typename T>\
                              friend QObject* NewObject(); \
                              private:
class QFAClass;
// use in .cpp 
#define QFAClassOut(className) QFAClassInfo<className> className::_QFAClassInfo;

extern "C" __declspec(dllexport) void* ___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___(QFAClass** engineClasses);
// free engine calsses before FreeLibrary
extern "C" __declspec(dllexport) void ___QFAGAMECODEEXPORTFUNCTIONFreeClasses___();

class GameCodeCompiler;


/*




delete all QFAClass(QCI) from this module if hotreload


*/
class QFAGameCode;
class QFAOverlord;
/*
    only this class and child class can change some inside
*/
class QFAClass
{
    friend void* ___QFAGAMECODEEXPORTFUNCTIONGETFUNCTIONS___(QFAClass** engineClasses);
    friend void ___QFAGAMECODEEXPORTFUNCTIONFreeClasses___();
    friend QFAOverlord;
    friend QFAGameCode;
public:
    enum ObjectClasses : int // if create new engine object class not forget add in ObjectClasses and QFAClass::InitClasses
    { // id is index in QCI
        Undefined = -1,
        Object = 0,
        Actor = 1,
        ActorComponent = 2,
        SceneComponent = 3,
        StaticMeshActor = 4,
        CameraActor = 5,
        CameraComponent = 6,
        MeshBase = 7,
        StaticMesh = 8,
        World = 9,
        DirectionLight = 10,
        AudioSceneComponent = 11,
        MAX
    };
    static bool ClassInit ;
#if In_Game_Module
    static const bool GameModuleClass = true;
#else
    static const bool GameModuleClass = false;
#endif
protected:
    const char* ClassName;
    const char* ClassRawName;
    bool IsAbstract;

    // first ObjectClasses::MAX classes is engine class
    static std::vector<QFAClass*> QCI;
#if QFA_EDITOR_ONLY
    static std::vector<QFAClass*> QCIOnlyGameClass; 

    std::vector<QObject*> ObjectList;

#endif

    /*
        this id temporary, can change between compilations
    */
    ObjectClasses ClassId = ObjectClasses::Undefined;
    ObjectClasses BaseOn = ObjectClasses::Undefined;
    ObjectClasses EngineClass = ObjectClasses::Undefined; // if Undefined this not engine class


private:


protected:
    static void InitClasses(QFAClass** engineClasses = nullptr);
    virtual QObject* CreateObjectInside() { return nullptr; };
    static QObject* CreateObject(size_t classId);
    static QObject* CreateObjectByName(const char* className);
    static void DeleteObject(QObject* object);
    static std::vector<QFAClass*>& GetGameClassList();
    QFAEXPORT static QFAClass** GetEngineClassList();// export for editor
    static std::vector<QFAClass*>& GetClassList();
    

#if QFA_EDITOR_ONLY
    static std::vector<QObject*>& GetListObject(size_t classId);
#endif

    QFAClass()
    {
        if (!QCI.size()) // for ObjectClasses
            QCI.resize(QFAClass::ObjectClasses::MAX);
    }

public:
    static size_t GetGameClassCount();
    static size_t GetEngineClassCount();
    static size_t GetClassCount();
    
    /*
        game class start from ObjectClasses::MAX index
        do not delete QFAClass
    */
    static QFAClass* GetClass(int classId);
    // do not delete QFAClass
    static QFAClass* GetClass(QObject* object);

protected:
#if QFA_EDITOR_ONLY
    void SetCompileIndex(QObject* object, size_t index);
    size_t GetCompileIndex(QObject* object);

#endif

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

    inline ObjectClasses GetBaseOn()
    {
        return BaseOn;
    }    

    inline bool GetIsAbstract()
    {
        return IsAbstract;
    }
};


template<typename T>
class QFAClassInfo : public QFAClass
{
    friend void QFAClass::InitClasses(QFAClass** );
private:
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
        SetCompileIndex(object, ObjectList.size());
        ObjectList.push_back(object);   

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
    QFAClassInfo(bool insideUsage)
    {
        ClassName = typeid(T).name();
        ClassRawName = typeid(T).raw_name();
        IsAbstract = std::is_abstract<T>::value;
    }

public:
    QFAClassInfo()
    {
        static_assert(std::is_base_of<QObject, T>::value, "class " __FUNCTION__" must inherited from base QObject");        
        if (std::is_base_of<QActor, T>::value)
            BaseOn = ObjectClasses::Actor;
        else if (std::is_base_of<QActorComponent, T>::value)
            BaseOn = ObjectClasses::ActorComponent;
        else
            BaseOn = ObjectClasses::Object;

        IsAbstract = std::is_abstract<T>::value;
        ClassName = typeid(T).name();
        ClassRawName = typeid(T).raw_name();

        ClassId = (ObjectClasses)QCI.size();
        QCI.push_back(this);
    }
};

/*  need in Editor, not use in game    */    
struct QFAEXPORT QFAGameCodeFunctions
{
    QObject* (*CreateObject)(size_t classId);
    QObject* (*CreateObjectByName)(const char* className);
    void (*DeleteObject)(QObject* object); // can use QObject::Destroy() instead it
    /*
        return list of all Game and engine class based on QObject
    */
    std::vector<QFAClass*>& (*GetGameClassList)();
    std::vector<QFAClass*>& (*GetClassList)();

#if QFA_EDITOR_ONLY
    /*
        return list of all created object based on QObject ( Engine classes not includet )
    */
    std::vector<QObject*>& (*GetGameObjectList)(size_t ClassId);
#endif
};

// used only in engine
#define QFAEngineClassIn(className) private: friend QFAClass; static QFAClass* _QFAClassInfo; public: QFAClass* GetClass() override;  private:
  
// used only in engine
#define QFAEngineClassOut(className) QFAClass* className::_QFAClassInfo; QFAClass* className::GetClass()  { return className::_QFAClassInfo; }

