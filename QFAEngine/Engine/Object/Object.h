#pragma once

#include <string>


class QFAEditorUIActorList;
class QObject
{
	friend QFAEditorUIActorList;

	static const unsigned int ValidNumber = 167031;
	unsigned int Valid = ValidNumber;
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


