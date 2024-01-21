#pragma once
#include <Render/UI/UIUnit.h>
#include <Tools/Array.h>
class QFAViewport;
class QFAWindow;
class QFAUIUnit;

class QFAUIParentComponent : public QFAUIUnit
{
	friend QFAUIUnit;
	friend QFAViewport;
	friend QFAWindow;	
protected:
	QFAArray<QFAUIUnit*> Children;
	// call if AddUnit success
	virtual void NewUnit(QFAUIUnit* unit) = 0;
	// child call if his slot change
	virtual void MySlotChange(QFAUIUnit* unit) = 0;	
public:

	enum EOverflow : unsigned char
	{
		Visible = 0,
		Hidden = 1,
		HiddenHorizon = 2,
		HiddenVertical = 3
	};

	QFAUIParentComponent();

	void AddUnit(QFAUIUnit* unit);
	void removeUnit(QFAUIUnit* unit);
	inline EOverflow GetOverflow()
	{
		return Overflow;
	}

	inline void SetOverflow(EOverflow over)
	{
		Overflow = over;
	}
protected:
	void RemoveUnitWithoutNotify(QFAUIUnit* unit);
	EOverflow Overflow = EOverflow::Visible;

	
	/*
		if child class nedd this 4 function create new virtual function for them
	*/
	void ParentEnable() final;
	
	
	void ParentDisable() final;
	

	
	void ParentAttach() final;
	
	
	void ParentDisconect() final;
	
	/*--*/
};