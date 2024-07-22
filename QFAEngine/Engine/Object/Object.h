#pragma once
#include <Tools/Stuff.h>
#include <string>
#include <vector>
#include "Class.h"

template<typename T>
T* NewObject()
{	
	
	static_assert(std::is_base_of<QObject, T>::value, "class T must inherited from base QObject");	
	T* t = (T*)malloc(sizeof(T));	
	if (!t)
		return nullptr;

	return new (t) T();
}


class QFAEditorGameViewportInfo;
class QFAClass;
class QFAGameCode;
class QFAEXPORT QObject
{
	friend QFAClass;
	template<typename T>
	friend class QFAClassInfo;
	friend QFAEditorGameViewportInfo;
	template<typename T>
	friend T* NewObject();

	static const unsigned int ValidNumber = 167031;
	unsigned int Valid = ValidNumber;
	// need for QFAClass
	//QFAClass* Class = nullptr;

	// not delete operator new because with new call constructor in NewObject
	static void* operator new[](size_t) = delete;
	static void  operator delete  (void*) = delete;
	static void  operator delete[](void*) = delete;
	

#if QFA_EDITOR_ONLY
    friend QFAGameCode;
	// index in QFAClassInfo::ObjectListInside
	size_t CompileIndex;	

#endif // QFAEDITORONLY
protected:

	std::u32string Name;

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

	virtual QFAClass::ObjectClasses GetEngineClassId();

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

	virtual QFAClass* GetClass()
	{
		return QFAClass::GetClass(GetEngineClassId());
	}
	



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
