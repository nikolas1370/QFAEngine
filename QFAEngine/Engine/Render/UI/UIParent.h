#pragma once
#include <Render/UI/UIUnit.h>

class QFAUIParentMultipleUnit;
class QFAUIParentOneUnit;
class QFAUIUnit;
class QFAWindow;
class QFAUIEvent;
class QFAUIScroll;
class QFAUIParent : public QFAUIUnit
{
	friend QFAUIParentMultipleUnit;
	friend QFAUIParentOneUnit;
	friend QFAUIUnit;
	friend QFAWindow;
	friend QFAUIEvent;
	friend QFAUIScroll;

public:
	enum EOverflow : unsigned char
	{
		Visible = 0,
		Hidden = 1,
		HiddenHorizon = 2,
		HiddenVertical = 3
	};

protected:
	// child call if his slot change
	virtual void MySlotChange(QFAUIUnit* unit) = 0;
	/*

	unit be remove from this parent and unit not be Notify about
		use if unit be add to other parent or destroyed

*/
	virtual void RemoveUnitWithoutNotify(QFAUIUnit* unit) = 0;

	EOverflow Overflow = EOverflow::Visible;

	virtual float UpdateInnerHeight() = 0;

	// if true parent can have one child
	bool OneUnit = false;
public:
	QFAUIParent();
	~QFAUIParent();

	inline EOverflow GetOverflow()
	{
		return Overflow;
	}

	virtual inline void SetOverflow(EOverflow over)
	{
		Overflow = over;
	}
protected:

};
