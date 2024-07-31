#include "pch.h"
#include "Background.h"
#include <Tools/String.h>

QFAUIBackground::QFAUIBackground()
{	
	Type = QFAUIType::Background;
}

QFAUIBackground::~QFAUIBackground()
{
}

void QFAUIBackground::MySlotChange(QFAUIUnit* unit)
{
}

void QFAUIBackground::WidthChanged(int oldValue)
{
	if (Child)
	{
		Child->ParentSetWidth = Width;
		Child->SetWidth(Child->StrWidth, Child->ParentSetWidthMinus);
	}
}

void QFAUIBackground::HeightChanged(int oldValue)
{
	if (Child)
	{
		Child->ParentSetHeight = Height;
		Child->SetHeight(Child->StrHeight, Child->ParentSetHeightMinus);
	}
}

void QFAUIBackground::TopChanged(int oldValue)
{
	if (Child)
	{
		Child->ParentSetPosition_y = Position_y;
		Child->SetTop(Child->StrTop);
	}
}

void QFAUIBackground::LeftChanged(int oldValue)
{
	if (Child)
	{
		Child->ParentSetPosition_x = Position_x;
		Child->SetLeft(Child->StrLeft);
	}
}

float QFAUIBackground::UpdateInnerHeight()
{
	if(!Child->IsValid())
		return 0.0f;

	if (Child->CanBeParent)
	{
		QFAUIParent* parent = (QFAUIParent*)Child;
		return parent->UpdateInnerHeight();
	}
	else
		return Child->InnerHeight;
}

float QFAUIBackground::UpdateInnerWidth()
{
	if (!Child->IsValid())
		return 0.0f;

	if (Child->CanBeParent)
	{
		QFAUIParent* parent = (QFAUIParent*)Child;
		return parent->UpdateInnerWidth();
	}
	else
		return Child->InnerWidth;
}


void QFAUIBackground::ChangedUnit(QFAUIUnit* unit)
{	
	unit->SetTop(unit->StrTop);
	unit->SetLeft(unit->StrLeft);
	unit->SetWidth(unit->StrWidth, unit->ParentSetWidthMinus);
	unit->SetHeight(unit->StrHeight , unit->ParentSetHeightMinus);
}

