#include "pch.h"
#include "Grid.h"
#include <Tools/String.h>
QFAUIGrid::QFAUIGrid()
{
	Type = QFAUIType::Grid;
}

QFAUIGrid::~QFAUIGrid()
{

}

void QFAUIGrid::WidthChanged(int oldValue)
{
	ProcessChildPosition(ChildProcessType::EWidth);
}

void QFAUIGrid::HeightChanged(int oldValue)
{
	ProcessChildPosition(ChildProcessType::EHeight);
}

void QFAUIGrid::TopChanged(int oldValue)
{
	ProcessChildPosition(ChildProcessType::ETop);
}

void QFAUIGrid::LeftChanged(int oldValue)
{
	ProcessChildPosition(ChildProcessType::ELeft);
}

void QFAUIGrid::NewUnit(QFAUIUnit* unit) 
{	
	ProcessChildPosition(ChildProcessType::EAll);
}

void QFAUIGrid::UnitWasRemoved()
{
	ProcessChildPosition(ChildProcessType::EAll);
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

#define GetCountUnit(Width, MinUnitSize) floor((float)(Width) / (float)(MinUnitSize))
#define GetSizeUnit(Width, MinUnitSize) ((float)(Width) / GetCountUnit(Width, MinUnitSize))

int QFAUIGrid::GetSizeUnitAuto(int& columnCount)
{
	if (UnitSize > 0)
		return UnitSize;

	int offsetCount = (int)GetCountUnit(Width, MinUnitSize) - 1;
	int widthWithoutOffset = Width - ColumnOffset * offsetCount;
	unsigned int unitWidth = GetSizeUnit(widthWithoutOffset, MinUnitSize);
	columnCount = offsetCount + 1;
	if (columnCount)
		return widthWithoutOffset / columnCount;
	else
		return 0;	
}

int QFAUIGrid::GetSizeUnitCount()
{
	int widthWithoutOffset = Width - ColumnOffset * (ColumnCount - 1);
	return widthWithoutOffset / ColumnCount + 1;
}

void QFAUIGrid::ProcessChildPosition(ChildProcessType type)
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

		if (type == QFAUIGrid::ETop || type == QFAUIGrid::EAll)
		{
			Children[i]->ParentSetPosition_y = Position_y + (int)((float)(roowCount - 1) * ((float)unitWidth * Ratio + (float)RowOffset));
			Children[i]->SetTop(Children[i]->StrTop);
		}

		if (type == QFAUIGrid::ELeft || type == QFAUIGrid::EAll)
		{
			Children[i]->ParentSetPosition_x = Position_x + columnCount * (unitWidth + (int)ColumnOffset);
			Children[i]->SetLeft(Children[i]->StrLeft);
		}

		if (type == QFAUIGrid::EWidth || type == QFAUIGrid::EAll)
		{
			Children[i]->ParentSetWidth = unitWidth;
			Children[i]->SetWidth(Children[i]->StrWidth, Children[i]->ParentSetWidthMinus);
		}

		if (type == QFAUIGrid::EHeight || type == QFAUIGrid::EAll)
		{
			Children[i]->ParentSetHeight = (int)((float)unitWidth * Ratio);
			Children[i]->SetHeight(Children[i]->StrHeight, Children[i]->ParentSetHeightMinus);
		}

		columnCount++;
		if (columnCount == maxColumnCount)
			columnCount = 0;
	}
}

void QFAUIGrid::SetColumnCount(unsigned int columnCount)
{
	ColumnCount = columnCount;
	ProcessChildPosition(QFAUIGrid::EAll);
}

void QFAUIGrid::SetOffsets(unsigned int columnOffset, unsigned int rowOffset)
{
	ColumnOffset = columnOffset;
	RowOffset = rowOffset;
	ProcessChildPosition(QFAUIGrid::EAll);
}

void QFAUIGrid::SetUnitWidth(unsigned int unitWidth)
{
	UnitSize = unitWidth;
	ProcessChildPosition(QFAUIGrid::EAll);
}

void QFAUIGrid::SetMin(unsigned int min)
{
	MinUnitSize = min;
	ProcessChildPosition(QFAUIGrid::EAll);
}

void QFAUIGrid::SetRation(float ratio)
{
	Ratio = ratio;
	ProcessChildPosition(QFAUIGrid::EAll);
}

void QFAUIGrid::SetPositionType(UnitPositionType type)
{
	PositionTypeType = type;
	ProcessChildPosition(QFAUIGrid::EAll);
}