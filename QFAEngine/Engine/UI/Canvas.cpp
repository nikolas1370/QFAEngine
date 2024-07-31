#include "pch.h"
#include "Canvas.h"
#include <iostream>
#include <Tools/String.h>

QFAUICanvas::QFAUICanvas()
{
	Type = QFAUIType::Canvas;
}

QFAUICanvas::~QFAUICanvas()
{
}

void QFAUICanvas::NewUnit(QFAUIUnit* unit)
{	
	if (!unit->IsValid())
		return;

	unit->ParentSetPosition_x = Position_x;
	unit->ParentSetPosition_y = Position_y;
	unit->ParentSetWidth = Width;
	unit->ParentSetHeight = Height;
	unit->SetWidth(unit->StrWidth, unit->ParentSetWidthMinus);
	unit->SetHeight(unit->StrHeight, unit->ParentSetHeightMinus);
	unit->SetTop(unit->StrTop);
	unit->SetLeft(unit->StrLeft);
}

void QFAUICanvas::MySlotChange(QFAUIUnit* unit)
{
	NewUnit(unit);
}

void QFAUICanvas::WidthChanged(int oldValue)
{
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetWidth = Width;
		Children[i]->SetWidth(Children[i]->StrWidth, Children[i]->ParentSetWidthMinus);
	}
}

void QFAUICanvas::HeightChanged(int oldValue)
{
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetHeight = Height;
		Children[i]->SetHeight(Children[i]->StrHeight, Children[i]->ParentSetHeightMinus);
	}
}

void QFAUICanvas::TopChanged(int oldValue)
{
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetPosition_y = Position_y;
		Children[i]->SetTop(Children[i]->StrTop);
	}
}

void QFAUICanvas::LeftChanged(int oldValue)
{
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetPosition_x = Position_x;
		Children[i]->SetLeft(Children[i]->StrLeft);
	}
}

float QFAUICanvas::UpdateInnerHeight()
{	
	return (float)Height;
}

float QFAUICanvas::UpdateInnerWidth()
{
	return (float)Width;
}

