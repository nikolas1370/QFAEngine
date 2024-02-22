#include "UIParentMultipleUnit.h"
#include <Render/Window/Viewport.h>
#include <Render/UI/RenderUnit.h>

QFAUIParentMultipleUnit::QFAUIParentMultipleUnit()
{
	ParentType = EParentType::MultipleChild;
}

QFAUIParentMultipleUnit::~QFAUIParentMultipleUnit()
{
	for (size_t i = 0; i < Children.Length(); i++)
	{
		if (true)
		{
			Children[i]->Parent = nullptr;
		}
	}
}

void QFAUIParentMultipleUnit::AddUnit(QFAUIUnit* unit)
{
	if (!unit || unit == this)// check if unit is parent 
		return;

	if (unit->Parent)
		unit->Parent->RemoveUnitWithoutNotify(unit);
	
	Children.Add(unit);
	unit->Parent = this;
	unit->ParentAttach();
	NewUnit(unit);
	if (unit->CanRender)
		((QFAUIRenderUnit*)unit)->UnitScroll = 0;
}

void QFAUIParentMultipleUnit::removeUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;

	RemoveUnitWithoutNotify(unit);
	unit->ParentDisconect();
}

void QFAUIParentMultipleUnit::RemoveUnitWithoutNotify(QFAUIUnit* unit)
{
	Children.Remove(unit);
	unit->Parent = nullptr;
}


void QFAUIParentMultipleUnit::ParentEnable()
{	
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentEnable();
}

void QFAUIParentMultipleUnit::ParentDisable()
{
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentDisable();
}

void QFAUIParentMultipleUnit::ParentAttach()
{
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentAttach();
}

void QFAUIParentMultipleUnit::ParentDisconect()
{
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentDisconect();
}
