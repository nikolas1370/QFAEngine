#include "Grid.h"

QFAUIGrid::QFAUIGrid()
{
	Type = QFAUIType::Grid;
	SelfResizable = true;
}

void QFAUIGrid::NewUnit(QFAUIUnit* unit) 
{	
	ProcessChildPosition();
}

#define GetCountUnit(Width, MaxUnitSize) (unsigned int)ceill((float)(Width) / (MaxUnitSize))
#define GetSizeUnit(Width, MaxUnitSize) ((Width) / GetCountUnit(Width, MaxUnitSize))

int QFAUIGrid::GetSizeUnitAuto(int& columnCount)
{
	if (UnitSize > 0)
		return UnitSize;
	else if (MaxUnitSize == 0)
		return 1;

	int  offsetCount = (GetCountUnit(Width, MaxUnitSize + ColumnOffset) - 1);
	int widthWithoutOffset = Width - ColumnOffset * offsetCount;
	int unitWidth = GetSizeUnit(widthWithoutOffset, MaxUnitSize);
	if (unitWidth >= MinUnitSize)
	{
		columnCount = GetCountUnit(widthWithoutOffset, MaxUnitSize);
		return unitWidth + 1;
	}
	else
	{
		columnCount = GetCountUnit(Width, MaxUnitSize);
		return GetSizeUnit(Width, MaxUnitSize) + 1;
	}
}

int QFAUIGrid::GetSizeUnitCount()
{
	int widthWithoutOffset = Width - ColumnOffset * (ColumnCount - 1);
	return widthWithoutOffset / ColumnCount + 1;
}

void QFAUIGrid::ProcessChildPosition()
{
	int unitWidth;
	int maxColumnCount = ColumnCount;

	if (PositionTypeType == UnitPositionType::UPTAuto)
		unitWidth = GetSizeUnitAuto(maxColumnCount);
	else if (PositionTypeType == UnitPositionType::UPTColumCount)
	{
		maxColumnCount = ColumnCount;
		unitWidth = GetSizeUnitCount();
	}
	else
		unitWidth = Width;

	int roowCount = 0;
	int columnCount = 0;
	for (size_t i = 0; i < Children.Length(); i++)
	{

		Children[i]->SetSizeParent(unitWidth, unitWidth * Ratio);
		Children[i]->SetPositionParent(
			Position_x + columnCount * (unitWidth + ColumnOffset),
			Position_y + (roowCount * (unitWidth * Ratio + RowOffset)));

		columnCount++;
		if (columnCount == maxColumnCount )
		{			
			columnCount = 0;
			roowCount++;
		}		
	}
}

void QFAUIGrid::SetSizeParent(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	ProcessChildPosition();
}

void QFAUIGrid::SetPositionParent(unsigned int x, unsigned int y)
{
	Position_x = x;
	Position_y = y;
	ProcessChildPosition();
}

void QFAUIGrid::SetColumnCount(unsigned int columnCount)
{
	ColumnCount = columnCount;
	ProcessChildPosition();
}

void QFAUIGrid::SetOffsets(unsigned int columnOffset, unsigned int rowOffset)
{
	ColumnOffset = columnOffset;
	RowOffset = rowOffset;
	ProcessChildPosition();
}

void QFAUIGrid::SetUnitWidth(unsigned int unitWidth)
{
	UnitSize = unitWidth;
	ProcessChildPosition();
}

void QFAUIGrid::SetMinMax(unsigned int min, unsigned int max)
{
	MinUnitSize = min;
	MaxUnitSize = max;
	ProcessChildPosition();
}

void QFAUIGrid::SetRation(float ratio)
{
	Ratio = ratio;
	ProcessChildPosition();
}

void QFAUIGrid::SetPositionType(UnitPositionType type)
{
	PositionTypeType = type;
	ProcessChildPosition();
}