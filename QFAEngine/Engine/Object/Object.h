#pragma once
#include <Tools/Stuff.h>
#include <string>
#include <vector>

#if true
	#define QSuper __super // for vs
#else
	#define QSuper(NearestQBaseClass) NearestQBaseClass // for other
#endif 

class QFAEditorGameViewportInfo;
class QFAClass;
class QFAGameCode;
class QFAClassInstance;
class QFAEngineClassInstance;
class QFAEXPORT QObject
{
	friend QFAClass;
	friend QFAClassInstance;
	friend QFAEngineClassInstance;
	template<typename T>
	friend class QFAClassInfo;
	friend QFAEditorGameViewportInfo;	
	template<typename T>
	friend T* NewObject();


	static const unsigned int ValidNumber = 167031;
	unsigned int Valid = ValidNumber;

	// not delete operator new because with new call constructor in NewObject
	static void* operator new[](size_t) = delete;
	static void  operator delete  (void*) = delete;
	static void  operator delete[](void*) = delete;
	static QFAClass* _QFAClassInfo; 

#if QFA_EDITOR_ONLY
    friend QFAGameCode;

	// true if object be creatre with api in editor
	bool CreateInApi = false;
#endif // QFAEDITORONLY
protected:
	/*----- QFACLASS -----*/
	std::u32string Name;
	static QFAClass* GetClassStatic();
	static QFAClass* GetParentClass() { return nullptr; }
public:

	virtual QFAClass* GetClass();
	/*----- QC END -----*/

private:


#if QFA_EDITOR_ONLY
    /*
        use for HotReload it's only for engine class
        in curent time use only for QActor, not be used for actorComponent
    */
    virtual void ReplaceMe(QObject* newObject) 
    {
		newObject->~QObject();
		free(newObject); 
    };

#endif // QFAEDITORONLY

	

protected:
	QObject();
	/*
* about virtual destructor
* 	https://isocpp.org/wiki/faq/virtual-functions#virtual-dtors
* crate QActor and cast pointer to QObject and delete this
* with virtual destructor QActor be call
* whitout virtual not be call
*/
	virtual ~QObject();
public:

	inline bool IsValid()
	{
		return this && Valid == ValidNumber;
	}

	/*
		Every class that inherits from QObject must delete self from parent.
		QActor delete self from QWorld
		QSceneComponent delete from QSceneComponent and QActor
	*/
	virtual void Destroy() final;
	
	inline std::u32string GetName()
	{
		return Name;
	}
};


template<typename T>
T* NewObject()
{
	static_assert(std::is_base_of<QObject, T>::value, "class T must inherited from base QObject");
	T* t = (T*)malloc(sizeof(T));
	if (!t)
		stopExecute("cannot malloc");

	return new (t) T();
}

QFAEXPORT QObject* NewObject(int classId);