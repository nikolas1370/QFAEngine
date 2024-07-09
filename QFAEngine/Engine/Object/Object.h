#pragma once
#include <Tools/Stuff.h>
#include <string>
#include <vector>
#include "Class.h"

class QFAEditorGameViewportInfo;
class QFAClass;
class QFAGameCode;
class QFAEXPORT QObject
{
	friend QFAEditorGameViewportInfo;
	friend QFAClass;
    
	static const unsigned int ValidNumber = 167031;
	unsigned int Valid = ValidNumber;
	// need for QFAClass
	QFAClass* Class; // if wanna use QFAClass::GetClass

#if QFA_EDITOR_ONLY
    friend QFAGameCode;
	// index in QFAClassInfo::ObjectListInside
	size_t CompileIndex;	
    bool CreateInGameCode = false;
#endif // QFAEDITORONLY
protected:

	std::u32string Name;

private:

#if QFA_EDITOR_ONLY
    QObject(bool createInGameCode)
    {
        CreateInGameCode = createInGameCode;        
    }
    /*
        use for HotReload it's only for engine class
        in curent time use only for QActor, not be used for actorComponent
    */
    virtual void ReplaceMe(QObject* newObject) 
    {
        delete newObject;
    };
#endif // QFAEDITORONLY

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
