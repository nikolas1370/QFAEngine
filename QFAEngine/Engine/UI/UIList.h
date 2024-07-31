#pragma once
#include <UI/UIParentMultipleUnit.h>

class QFAEXPORT QFAUIList : public QFAUIParentMultipleUnit
{
	enum ChildCalculateType
	{
		EAll,
		ETop,
		ELeft,
		EWidth,
		EHeight
	};

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

private:
	void CalculateChildren(ChildCalculateType cct);
	void ChildInnerChange(QFAUIUnit* child) override;
	void UnitWasRemoved() override;

protected:
	void NewUnit(QFAUIUnit* unit) override;
	void MySlotChange(QFAUIUnit* unit) override;


	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	
	// if true last unit be have height or width like free space in list

public:
	QFAUIList();
	~QFAUIList();

	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;

	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

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
};
