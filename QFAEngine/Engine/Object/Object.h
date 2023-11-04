#pragma once

#include <string>



class QObject
{
	static const unsigned int ValidNumber = 167031;
	unsigned int Valid = ValidNumber;
protected:

	
public:
	QObject();

	std::string Name;

	/*
	* about virtual destructor
	* 	https://isocpp.org/wiki/faq/virtual-functions#virtual-dtors
	* crate QActor and cast pointer to QObject and delete this 
	* with virtual destructor QActor be call
	* whitout virtual not be call
	*/
	virtual ~QObject();


	bool IsValid();

	/*
		analogue to "delete someQObject;"
	*/
	virtual void Destroy() final;
	

};


