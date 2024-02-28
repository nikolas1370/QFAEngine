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

float QFAUIGrid::UpdateInnerHeight()
{
	if (roowCount <= 0)
		InnerHeight = 0;
	else if (roowCount == 1)
		InnerHeight = (unsigned int)((float)unitWidth * Ratio);
	else
		InnerHeight = (unsigned int)(((float)unitWidth * Ratio) * roowCount + (roowCount - 1) * RowOffset);

	return (float)InnerHeight;
}

float QFAUIGrid::UpdateInnerWidth()
{
	return (float)Width;
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
	unsigned int unitWidth = GetSizeUnit(widthWithoutOffset, MaxUnitSize);
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
	unitWidth = 0;
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

	roowCount = 0;
	int columnCount = 0;
	for (size_t i = 0; i < Children.Length(); i++)
	{
		if (columnCount == 0)
			roowCount++;

		Children[i]->SetSizeParent((unsigned int)unitWidth, (unsigned int)((float)unitWidth * Ratio));
		Children[i]->SetPositionParent(
			Position_x + columnCount * (unitWidth + (int)ColumnOffset),
			Position_y + (int)((float)(roowCount - 1) * ((float)unitWidth * Ratio + (float)RowOffset)));

		columnCount++;
		if (columnCount == maxColumnCount)
			columnCount = 0;
	}
}

void QFAUIGrid::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	ProcessChildPosition();
}

void QFAUIGrid::ChangePosition(int x, int y)
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