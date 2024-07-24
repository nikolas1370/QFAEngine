#pragma once
#include <UI/UIParentMultipleUnit.h>

class QFAEXPORT QFAUIList : public QFAUIParentMultipleUnit
{
public:
	enum EListType
	{
		LTVertical,
		LTHorizon,
		/* right now work only for text */
		LTHorizonInner, // instead UnitWidth use childUnit.innerWidth
		LTVerticalInner // instead UnitHeight use childUnit.innerHeight
	};

private:
	EListType ListType = EListType::LTVertical;
	unsigned int UnitHeight = 50;
	unsigned int UnitWidth = 150;

protected:
	bool StretchLastUnit = false;

private:
	void CalculateChildren();
	void ChildInnerChange(QFAUIUnit* child) override;
	void UnitWasRemoved() override;

protected:
	void NewUnit(QFAUIUnit* unit) override;
	void MySlotChange(QFAUIUnit* unit) override;

	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	
	// if true last unit be have height or width like free space in list

public:
	QFAUIList();
	~QFAUIList();

	void SetListType(EListType listType);
	inline EListType GetListType()
	{
		return ListType;
	}

	// use UnitHeight if ListType == LTVertical;
	void SetUnitHeight(unsigned int h);
	// use UnitWidth if ListType == LTHorizon;
	void SetUnitWidth(unsigned int w);

	inline void SetOverflow(EOverflow over) override {};
	inline void SetStretchLastUnit(bool enable)
	{
		StretchLastUnit = enable;
	}
};