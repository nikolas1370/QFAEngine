#pragma once
#include <iostream>

#include <string>
#include <vector>

class QFAEditorGameViewportInfo;
class QFAClass;
class QObject
{
	friend QFAEditorGameViewportInfo;
	friend QFAClass;

	static const unsigned int ValidNumber = 167031;
	unsigned int Valid = ValidNumber;
	// ClassId in GameCode.dll
	size_t ClassId;

#if QFA_EDITOR_ONLY
	// index in QFAClass::ObjectArray
	size_t CompileIndex;	
    bool CreateInGameCode = false;
    QObject(bool createInGameCode)
    {
        CreateInGameCode = createInGameCode;
    }
#endif // QFAEDITORONLY

    
protected:

	std::u32string Name;



public:
	QObject();

	

	/*
	* about virtual destructor
	* 	https://isocpp.org/wiki/faq/virtual-functions#virtual-dtors
	* crate QActor and cast pointer to QObject and delete this 
	* with virtual destructor QActor be call
	* whitout virtual not be call
	*/
	virtual ~QObject();


	inline bool IsValid()
	{
		return this && Valid == ValidNumber;
	}

	/*
		analogue to "delete someQObject;"
	*/
	virtual void Destroy() final;
	
	inline std::u32string GetName()
	{
		return Name;
	}
};

/* 
*
* 
* 
* game class
* 
* 
* 
*/
// use inside QObject class
#define QFAClassIn(className) static QFAClassInfo<className> _QFAClassInfo;

// use in .cpp 
#define QFAClassOut(className) QFAClassInfo<className> className::_QFAClassInfo;
/* use for editor game viewport */

class QFAClass;
struct QFAGameCodeFunctions
{
    QObject* (*CreateObject)(size_t classId);
    void (*DeleteObject)(QObject* object);
    std::vector<QFAClass*>& (*GetGameClassList)();
};

extern "C" __declspec(dllexport) QFAGameCodeFunctions * QFAGetFunctions();

class GameCodeCompiler;
/*
    only this class and child class can change some inside
*/
class QFAClass
{
    friend QFAGameCodeFunctions* QFAGetFunctions();
    static QFAGameCodeFunctions QFCFs;
protected:
    static int QfaClassCount;
    /*
        0 is reserved first id 1
        this id temporary, can change between compilations
    */
    size_t ClassId; 
    static std::vector<QFAClass*> QCI;
    const char* ClassName;
    const char* ClassRawName;

#if QFA_EDITOR_ONLY
    /*
        list of pointer to created T object
        if code recompile all object in ObjectArray be replase to new version this object
    */
    std::vector<QObject*> ObjectArray;
#endif // QFAEDITORONLY

    virtual QObject* CreateObjectInside() = 0;
    virtual void DeleteObjectInside(QObject* object) = 0;

    static QObject* CreateObject(size_t classId)
    {        
        if (QCI.size() < classId)
        {
            std::cout << "QFAClass::CreateObject nulptr\n";
            return nullptr;
        }

        return QCI[classId - 1]->CreateObjectInside();
    }

    static void DeleteObject(QObject* object)
    {
        if (!object->IsValid())
            return;

        QCI[object->ClassId - 1]->DeleteObjectInside(object);
    }

    static std::vector<QFAClass*>& GetGameClassList()
    {
        return QCI;
    }

    inline void SetCompileIndex(QObject* object, size_t index)
    {
        object->CompileIndex = index;
    }

    inline size_t GetCompileIndex(QObject* object)
    {
        return object->CompileIndex;
    }

    inline void SetClassId(QObject* object, size_t classId)
    {
        object->ClassId = classId;
    }
public:
    
    const char* GetClassName()
    {
        return ClassName;
    }

    const char* GetClassRawName()
    {
        return ClassRawName;
    }

    size_t GetClassId()
    {
        return ClassId;
    }        

    static QFAClass* GetClass(size_t classId)
    {
        if (QCI.size() < classId)
        {
            std::cout << "QFAClass::GetClass nulptr\n";
            return nullptr;
        }

        return QCI[classId - 1];
    }
};

class QActor;
class QSceneComponent;
class QActorComponent;
template<typename T>
class QFAClassInfo : public QFAClass // QFAClassInfoBase
{
public:
    QFAClassInfo()
    {// if false compile error        
        static_assert(std::is_base_of<QObject, T>::value, "class " __FUNCTION__" must inherited from base QObject");
        
        ClassName = typeid(T).name();
        ClassRawName = typeid(T).raw_name();
        QCI.push_back(this);
        ClassId = ++QfaClassCount;
    }
private:
#if QFA_EDITOR_ONLY
    QObject* CreateObjectInside() override
    {
        QObject* object = new T();
        SetCompileIndex(object, ObjectArray.size());
        ObjectArray.push_back(object);
        SetClassId(object, ClassId);
        return object;
    }
#else
    QObject* CreateObjectInside() override
    {
        QObject* object = new T();
        SetClassId(object, ClassId);
        return object;
    }
#endif // QFAEDITORONLY

#if QFA_EDITOR_ONLY
    void DeleteObjectInside(QObject* object) override
    {
        if (ObjectArray.size() <= GetCompileIndex(object) ||
            ObjectArray[GetCompileIndex(object)] != object)
        {
            std::cout << "QFAClassInfo::DeleteObject " << !object->IsValid() << " " << (ObjectArray.size() <= GetCompileIndex(object)) << " " << (ObjectArray[GetCompileIndex(object)] != object) << " -----------------------------\n";
            return;
        }

        size_t index = GetCompileIndex(object);
        delete object;
        ObjectArray[index] = ObjectArray.back();
        SetCompileIndex(ObjectArray[index], index);
        ObjectArray.pop_back();
    }
#else
    void DeleteObjectInside(QObject* object) override
    {
        if (!object->IsValid())
            return;
        else
            delete object;
    }
#endif // QFAEDITORONLY

};