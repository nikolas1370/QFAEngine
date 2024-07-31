#pragma once
#include "UIParentMultipleUnit.h"

class  QFAEXPORT QFAUIGrid : public QFAUIParentMultipleUnit
{
	enum ChildProcessType
	{
		EAll,
		ETop,
		ELeft,
		EWidth,
		EHeight
	};
public:
	/*
		UPTAuto 
			if UnitSize == 0	
				UIUnit size be not less MinUnitSize
			else
				UIUnit size = UnitSize
			
		UPTColumCount
			amount unit in row equal ColumnCount. Every unit have same size
	
	*/
	enum UnitPositionType
	{
		UPTAuto = 0,
		UPTColumCount = 1 
	};

private:
	UnitPositionType PositionTypeType = UnitPositionType::UPTAuto;
	unsigned int ColumnCount = 3;
	unsigned int ColumnOffset = 0;
	unsigned int RowOffset = 0;
	unsigned int UnitSize = 0;
	unsigned int MinUnitSize = 100;
	float Ratio = 1;

	// not for setup grid
	int roowCount = 0;
	int unitWidth = 0;

	int GetSizeUnitAuto(int& columnCount);
	int GetSizeUnitCount();

	// positionX == true child positionX be change
	void ProcessChildPosition(ChildProcessType type);

	void NewUnit(QFAUIUnit* unit) override;
	void UnitWasRemoved() override;
	// child call if his slot change
	void MySlotChange(QFAUIUnit* unit) override {};

	float UpdateInnerHeight() override;
	// grid always return own Width
	float UpdateInnerWidth() override;
protected:
	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:

	 QFAUIGrid();
	 ~QFAUIGrid() {};


	void SetColumnCount(unsigned int columnCount);
	void SetOffsets(unsigned int columnOffset, unsigned int rowOffset);
	void SetUnitWidth(unsigned int unitWidth);
	void SetMin(unsigned int min);
	/*
	*  UnitWidth * Ratio == UnitHeight
	*  Height / Width
	*/
	void SetRation(float ratio);
	void SetPositionType(UnitPositionType type);
};