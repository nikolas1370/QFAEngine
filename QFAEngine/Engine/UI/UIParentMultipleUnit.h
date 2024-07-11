#pragma once
#include <UI/UIParent.h>
#include <Tools/Array.h>
class QFAEngineViewport;
class QFAEngineWindow;
class QFAUIUnit;
class QFAUIEvent;
class QFAUISelectUnit;
class QFAEXPORT QFAUIParentMultipleUnit : public QFAUIParent
{
	friend QFAUIUnit;
	friend QFAEngineViewport;
	friend QFAEngineWindow;	
	friend QFAUIEvent;
	friend QFAUISelectUnit;
protected:
	QFAArray<QFAUIUnit*> Children;

	// call if AddUnit success
	virtual void NewUnit(QFAUIUnit* unit) = 0;
	// call if removeUnit or removeAllUnit success
	virtual void UnitWasRemoved() {};
	void RemoveUnitWithoutNotify(QFAUIUnit* unit) final;

	/*
		if child class nedd this 4 function create new virtual function for them
	*/
	void ParentEnable() final;
	void ParentDisable() final;
	void ParentAttach() final;
	void ParentDisconect() final;

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
};