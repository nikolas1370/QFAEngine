#pragma once
#include "UIParentComponent.h"

class  QFAUIGrid : public QFAUIParentComponent
{
public:
	/*
		UPTAuto 
			if UnitSize == 0	
				UIUnit size be between MinUnitSize and MaxUnitSize
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

	 QFAUIGrid();
	 ~QFAUIGrid() {};

	
	void SetColumnCount(unsigned int columnCount);
	void SetOffsets(unsigned int columnOffset, unsigned int rowOffset);
	void SetUnitWidth(unsigned int unitWidth);
	void SetMinMax(unsigned int min, unsigned int max);
	/*
	*  UnitWidth * Ratio == UnitHeight
	*  Height / Width
	*/
	void SetRation(float ratio);
	void SetPositionType(UnitPositionType type);
	
private:

	UnitPositionType PositionTypeType = UnitPositionType::UPTAuto;
	unsigned int ColumnCount = 3;
	unsigned int ColumnOffset = 0;
	unsigned int RowOffset = 0;
	unsigned int UnitSize = 0;
	unsigned int MinUnitSize = 0;
	unsigned int MaxUnitSize = 1;	
	float Ratio = 1; 


	int GetSizeUnitAuto(int& columnCount);
	int GetSizeUnitCount();

	void ProcessChildPosition();
	
	void SetSizeParent(unsigned int w, unsigned int h) override;
	void SetPositionParent(unsigned int x, unsigned int y) override;

	void NewUnit(QFAUIUnit* unit) override;
	// child call if his slot change
	void MySlotChange(QFAUIUnit* unit) override {};
};