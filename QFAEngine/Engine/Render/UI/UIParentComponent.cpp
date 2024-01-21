#include "UIParentComponent.h"
#include <Render/Window/Viewport.h>

QFAUIParentComponent::QFAUIParentComponent()
{
	CanBeParent = true;
}

void QFAUIParentComponent::AddUnit(QFAUIUnit* unit)
{
	if (!unit || unit == this)// check if unit is parent 
		return;

	if (unit->Parent)
		unit->Parent->RemoveUnitWithoutNotify(unit);
	else if (unit->ParentViewport)
		unit->ParentViewport->RemoveUnitWithoutNotify(unit);
	else
		unit->ParentAttach();

	Children.Add(unit);
	unit->Parent = this;
	NewUnit(unit);
}

void QFAUIParentComponent::removeUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;

	RemoveUnitWithoutNotify(unit);
	unit->ParentDisconect();
}

void QFAUIParentComponent::RemoveUnitWithoutNotify(QFAUIUnit* unit)
{
	Children.Remove(unit);
	unit->Parent = nullptr;
}

void QFAUIParentComponent::ParentEnable()
{	
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentEnable();
}

void QFAUIParentComponent::ParentDisable()
{
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentDisable();
}

void QFAUIParentComponent::ParentAttach()
{
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentAttach();
}

void QFAUIParentComponent::ParentDisconect()
{
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->ParentDisconect();
}
