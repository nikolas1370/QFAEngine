#pragma once

// name before QFAObjectClasses
enum QFAObjectClasses : int // if create new engine object class not forget add in QFAObjectClasses and QFAClassInstance::InitClasses
{ // id is index in QCI
    QOCUndefined = -1,
    QOCObject = 0,
    QOCActor = 1,
    QOCActorComponent = 2,
    QOCSceneComponent = 3,
    QOCStaticMeshActor = 4,
    QOCCameraActor = 5,
    QOCCameraComponent = 6,
    QOCMeshBase = 7,
    QOCStaticMesh = 8,
    QOCWorld = 9,
    QOCDirectionLight = 10,
    QOCAudioSceneComponent = 11,
    QOCFlyingCamera = 12,
    QOCMAX
};

class QObject;
class QFAClass;
class QFAOverlord;
class QFAGameCode;
class QFAClassInstance;
class QFAEXPORT QFAEngineClassInstance
{
    friend QFAOverlord;
    friend QFAGameCode;
	static QFAEngineClassInstance* GameModuleClass;
    static QFAClassInstance* EngineeClassInstance;
    // store engine classes
	std::vector<QFAClass*> EQCI;

protected:
    // only game class
    std::vector<QFAClass*> GameQCI;

private:
	// only for engine
	static void InitClassesEngine() ;
    // not call in engine
    virtual void InitClasses(std::vector<QFAClass*> engineClasses) {};
    static std::vector<QFAClass*>& GetEngineClass();

protected:
    QFAEngineClassInstance();
	static void SetGameClassInstance(QFAEngineClassInstance* localInstance);
	
    virtual std::vector<QFAClass*>& GetClassList();
    std::vector<QFAClass*>& GetGameClassList();
    void BuildClassesTree(std::vector<QFAClass*>& classes);
    QObject* CreateObjectInside(QFAClass* clas);
    QObject* CreateObjectByNameInside(const char* className, std::vector<QFAClass*>& classes);

public:

    virtual QObject* CreateObject(size_t classId) { return nullptr; }
    virtual QObject* CreateObjectByName(const char* className) { return nullptr; }
    void DeleteObject(QObject* object);

    // only engine and editor can use it. if wanna instead can use QFAClassInstance::GetClassInstance()
    static QFAEngineClassInstance* GetGameClassInstance();
};

/*--- only for engine ---*/

/*
	if add new engine class need add in QFAClass::InitClasses
	and in QFAClass::QFAObjectClasses.
	used only in engine
*/
#define QFAEngineClassIn(EngineClassName) \
private: \
    friend QFAClass; \
    template<typename T>\
    friend class QFAClassInfo;\
    friend QFAEngineClassInstance;\
    static QFAClass* _QFAClassInfo;/* _QFAClassInfo need store in engine side */\
protected:\
    static QFAClass* GetParentClass() { return __super::GetClassStatic(); }  \
    static QFAClass* GetClassStatic();\
public: \
    QFAClass* GetClass() override;\
private:

#define QFAEngineClassOut(EngineClassName)\
    QFAClass* EngineClassName::_QFAClassInfo;\
    QFAClass* EngineClassName::GetClass() \
    { return EngineClassName::_QFAClassInfo; }\
    QFAClass* EngineClassName::GetClassStatic()\
    { return EngineClassName::_QFAClassInfo; };