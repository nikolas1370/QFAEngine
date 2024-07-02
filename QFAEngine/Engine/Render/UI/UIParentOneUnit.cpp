#include "pch.h"
#include "UIParentOneUnit.h"
#include <Render/UI/RenderUnit.h>

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
		Child->SetSizeParent(Width, Height);
		Child->SetPositionParent(Position_x, Position_y);
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
