#include "pch.h"
#include "UIParentOneUnit.h"
#include <UI/RenderUnit.h>

QFAUIParentOneUnit::QFAUIParentOneUnit()
{
	ParentType = EParentType::OneChild;
}

QFAUIParentOneUnit::~QFAUIParentOneUnit()
{
	if (Child)
	{
		Child->Parent = nullptr;
		Child = nullptr;
	}
}

void QFAUIParentOneUnit::SetUnit(QFAUIUnit* unit)
{
	if (Child == unit)
		return;

	if (Child)
	{
		Child->Parent = nullptr;
		Child->ParentDisconect();
	}

	Child = unit;
	if (Child)
	{
		if (unit->Parent)
			unit->Parent->RemoveUnitWithoutNotify(unit);

		Child->Parent = this;
		Child->ParentAttach();

		Child->ParentSetPosition_x = Position_x;
		Child->ParentSetPosition_y = Position_y;
		Child->ParentSetWidth = Width;
		Child->ParentSetHeight = Height;
		Child->SetWidth(Child->StrWidth, Child->ParentSetWidthMinus);
		Child->SetHeight(Child->StrHeight, Child->ParentSetHeightMinus);
		Child->SetTop(Child->StrTop);
		Child->SetLeft(Child->StrLeft);
		if(Child->CanRender)
			((QFAUIRenderUnit*)Child)->UnitScroll = 0;
	}
	
	ChangedUnit(unit);
}


void QFAUIParentOneUnit::ParentEnable()
{
	if(Child)
		Child->ParentEnable();
}

void QFAUIParentOneUnit::ParentDisable()
{
	if (Child)
		Child->ParentDisable();
}

void QFAUIParentOneUnit::ParentAttach()
{
	if (Child)
		Child->ParentAttach();
}

void QFAUIParentOneUnit::ParentDisconect()
{
	if (Child)
		Child->ParentDisconect();
}

void QFAUIParentOneUnit::RemoveUnitWithoutNotify(QFAUIUnit* unit)
{
	Child = nullptr;
	unit->Parent = nullptr;
}
