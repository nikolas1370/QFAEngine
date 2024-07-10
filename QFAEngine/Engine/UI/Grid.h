#pragma once
#include "UIParentMultipleUnit.h"

class  QFAEXPORT QFAUIGrid : public QFAUIParentMultipleUnit
{
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

	void ProcessChildPosition();

	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;

	void NewUnit(QFAUIUnit* unit) override;
	void UnitWasRemoved() override;
	// child call if his slot change
	void MySlotChange(QFAUIUnit* unit) override {};

	float UpdateInnerHeight() override;
	// grid always return own Width
	float UpdateInnerWidth() override;

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