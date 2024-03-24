#pragma once
#include <Render/UI/UIParent.h>
#include <Tools/Array.h>
class QFAViewport;
class QFAWindow;
class QFAUIUnit;
class QFAUIEvent;
//QFAUIParentUnit
class QFAUIParentMultipleUnit : public QFAUIParent
{
	friend QFAUIUnit;
	friend QFAViewport;
	friend QFAWindow;	
	friend QFAUIEvent;
	
protected:
	QFAArray<QFAUIUnit*> Children;
	// call if AddUnit success
	virtual void NewUnit(QFAUIUnit* unit) = 0;
	// call if removeUnit or removeAllUnit success
	virtual void UnitWasRemoved() {};
public:
	QFAUIParentMultipleUnit();
	~QFAUIParentMultipleUnit();

	void AddUnit(QFAUIUnit* unit);
	void removeUnit(QFAUIUnit* unit);
	void removeAllUnit();
	

	inline size_t GetUnitCount()
	{
		return Children.Length();
	}
protected:
	void RemoveUnitWithoutNotify(QFAUIUnit* unit) final;

	/*
		if child class nedd this 4 function create new virtual function for them
	*/
	void ParentEnable() final;
		
	void ParentDisable() final;
	
	void ParentAttach() final;
		
	void ParentDisconect() final;	

};